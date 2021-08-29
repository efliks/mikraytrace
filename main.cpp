#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "world.h"
#include "renderer.h"
#include "texture.h"
#include "config.h"


int main(int argc, char** argv) {
    bool quiet_flag = false;
    std::string png_file;
    std::vector<std::string> toml_files;
    mrtp::RendererConfig renderer_config;

    if (!(mrtp::process_command_line(
              argc,
              argv,
              &renderer_config,
              &toml_files,
              &png_file,
              &quiet_flag
              ))) {
        return 1;
    }

    bool use_auto_name = (toml_files.size() > 1) || (png_file == "");
    if (use_auto_name) {
        if (png_file != "") {
            std::cerr << "multiple toml files: do not use -o" << std::endl;
            return 1;
        }
    }

    // Textures will be shared by all worlds
    mrtp::TextureFactory texture_factory;

    // Iterate over all input files
    for (auto toml_file : toml_files) {
        if (!quiet_flag) {
            std::cout << "processing " << toml_file << std::flush;
        }

        auto my_world_ptr = mrtp::build_world(toml_file, &texture_factory);

        if (use_auto_name) {
            std::string foo(toml_file);
            size_t pos = toml_file.rfind(".toml");
            if (pos != std::string::npos) { foo = toml_file.substr(0, pos); }
            png_file = foo + ".png";
        }

        if (renderer_config.num_threads == 1) {
            mrtp::SceneRenderer scene_renderer(my_world_ptr.get(), renderer_config);
            mrtp::ScenePNGWriter scene_writer(&scene_renderer);

            scene_renderer.do_render();
            scene_writer.write_to_file(png_file);
        }
        else {
            mrtp::ParallelSceneRenderer scene_renderer(my_world_ptr.get(), renderer_config, renderer_config.num_threads);
            mrtp::ScenePNGWriter scene_writer(&scene_renderer);

            scene_renderer.do_render();
            scene_writer.write_to_file(png_file);
        }

        std::cout << " OK" << std::endl;
    }
    
    return 0;  // All done
}
