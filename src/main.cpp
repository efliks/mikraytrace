#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include "world.h"
#include "renderer.h"
#include "texture.h"
#include "writer.h"

#include <easylogging++.h>


int main(int argc, char* argv[])
{
    CLI::App app{"A simple raytracer"};

    std::vector<std::string> input_files;
    app.add_option("input_files", input_files, "Input file(s)")->mandatory();

    std::string output_file;
    app.add_option("-o,--output", output_file, "Output file");

    std::string output_format;
    app.add_option("-F,--format", output_format, "Output format")->default_val("png")->check(CLI::IsMember({"png", "jpg"}));

    mrtp::RendererConfig config;
    app.add_option("-f,--fov", config.field_of_vision, "Field of vision in degrees")->default_val(config.field_of_vision)->check(CLI::Range(50, 170));

    app.add_option("-W,--width", config.buffer_width, "Image width")->default_val(config.buffer_width)->check(CLI::Range(320, 3200));
    app.add_option("-H,--height", config.buffer_height, "Image height")->default_val(config.buffer_height)->check(CLI::Range(240, 2400));

    app.add_option("-r,--recursion", config.max_ray_depth, "Levels of recursion for reflected rays")->default_val(config.max_ray_depth)->check(CLI::Range(0, 5));
    app.add_option("-s,--shadow", config.shadow_bias, "Shadow coefficient")->default_val(config.shadow_bias);

    app.add_option("-t,--threads", config.num_threads, "Rendering threads (0 for auto)")->default_val(config.num_threads)->check(CLI::Range(0, 32));

    CLI11_PARSE(app, argc, argv);


    bool auto_name = input_files.size() > 1 || output_file.empty();
    if (auto_name && !output_file.empty()) {
        LOG(ERROR) << "Output file not allowed with multiple input files";
        return EXIT_FAILURE;
    }

    if (!output_file.empty()) {
        size_t pos = output_file.rfind(".");
        if (pos != std::string::npos) {
            std::string extension = output_file.substr(pos + 1, output_file.size());
            if (extension != output_format) {
                LOG(ERROR) << "Output format and output file extension should match";
                return EXIT_FAILURE;
            }
        }
    }

    // Textures will be shared by all worlds
    mrtp::TextureFactory texture_factory;

    // Iterate over all input files
    for (std::string& input_file : input_files) {
        LOG(INFO) << "Processing " << input_file << " ...";

        auto world_ptr = mrtp::build_world(input_file, &texture_factory);
        if (!world_ptr) {
            return EXIT_FAILURE;
        }

        if (auto_name) {
            std::string foo(input_file);
            size_t pos = input_file.rfind(".toml");  //FIXME
            if (pos != std::string::npos) {
                foo = input_file.substr(0, pos);
            }
            output_file = foo + "." + output_format;
        }

        mrtp::WriterType writer_type = (output_format == "png") ? mrtp::WriterType::PNG : mrtp::WriterType::JPEG;

        float render_t = 0;
        if (config.num_threads == 1) {
            mrtp::SceneRenderer scene_renderer(world_ptr.get(), config);
            //FIXME
            auto scene_writer = mrtp::create_writer(&scene_renderer, writer_type);

            render_t = scene_renderer.do_render();
            scene_writer->write_to_file(output_file);
        }
        else {
            mrtp::ParallelSceneRenderer scene_renderer(world_ptr.get(), config, config.num_threads);
            //FIXME
            auto scene_writer = mrtp::create_writer(&scene_renderer, writer_type);

            render_t = scene_renderer.do_render();
            scene_writer->write_to_file(output_file);
        }
        LOG(INFO) << "Done in " << std::setprecision(2) << render_t << "s";
    }
    
    return EXIT_SUCCESS;  // All done
}
