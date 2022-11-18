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
#include "logger.h"


int main(int argc, char* argv[])
{
    std::vector<std::string> input_files;

    std::string output_file;
    std::string output_format = "png";

    mrtp::RendererConfig config;


    CLI::App app{"A simple raytracer"};

    app.add_option("input_files", input_files, "Input file(s)")->mandatory();

    app.add_option("-o,--output", output_file, "Output file");
    app.add_option("-F,--format", output_format, "Output format")->default_val("png")->check(CLI::IsMember({"png", "jpg"}));

    app.add_option("-f,--fov", config.fov, "Field of vision in degrees")->default_val(config.fov)->check(CLI::Range(config.fov_min, config.fov_max));

    app.add_option("-W,--width", config.width, "Image width")->default_val(config.width)->check(CLI::Range(config.width_min, config.width_max));
    app.add_option("-H,--height", config.height, "Image height")->default_val(config.height)->check(CLI::Range(config.height_min, config.height_max));

    app.add_option("-t,--threads", config.num_thread, "Rendering threads (0 for auto)")->default_val(config.num_thread)->check(CLI::Range(config.num_min_thread, config.num_max_thread));

    CLI11_PARSE(app, argc, argv);


    bool auto_name = input_files.size() > 1 || output_file.empty();
    if (auto_name && !output_file.empty()) {
        LOG_ERROR("Output file not allowed with multiple input files");
        return EXIT_FAILURE;
    }

    if (!output_file.empty()) {
        size_t pos = output_file.rfind(".");
        if (pos != std::string::npos) {
            std::string extension = output_file.substr(pos + 1, output_file.size());
            if (extension != output_format) {
                LOG_ERROR("Output format and output file extension should match");
                return EXIT_FAILURE;
            }
        }
    }

    // Textures will be shared by all worlds
    mrtp::TextureFactory texture_factory;

    // Iterate over all input files
    for (std::string& input_file : input_files) {
        LOG_INFO(std::string("Processing " + input_file + " ..."));

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
        if (config.num_thread == 1) {
            mrtp::SceneRenderer scene_renderer(world_ptr.get(), config);
            //FIXME
            auto scene_writer = mrtp::create_writer(&scene_renderer, writer_type);

            render_t = scene_renderer.do_render();
            scene_writer->write_to_file(output_file);
        }
        else {
            mrtp::ParallelSceneRenderer scene_renderer(world_ptr.get(), config, config.num_thread);
            //FIXME
            auto scene_writer = mrtp::create_writer(&scene_renderer, writer_type);

            render_t = scene_renderer.do_render();
            scene_writer->write_to_file(output_file);
        }

        std::stringstream work_time;
        work_time << "Done in " << std::setprecision(2) << render_t << "s";

        LOG_INFO(work_time.str());
    }
    
    return EXIT_SUCCESS;  // All done
}
