#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>

#include "world.h"
#include "renderer.h"
#include "texture.h"


void help_message() {
    std::cout << R"(Usage: mrtp_cli [OPTION]... FILE...
  Options:
    -d   distance to darken light
    -f   field of vision in degrees
    -h   print this help screen
    -o   output filename in PNG format
    -q   suppress messages, except errors
    -r   resolution, eg. 640x480
    -R   levels of recursion for reflected rays
    -s   shadow factor
    -t   rendering threads: 0 (auto), 1, 2, ...

Example:
  mrtp_cli -r 1620x1080 -f 110.0 -o scene2.png scene2.toml)" << std::endl;
}


int main(int argc, char** argv) {
    int c;
    bool parse_ok = true;
    bool quiet_flag = false;

    std::string png_file;
    std::vector<std::string> toml_files;

    mrtp::RendererConfig renderer_config;

    if (argc < 2) {
        // no arguments - do exit
        help_message();
        return 0;
    }

    while ((c = getopt(argc, argv, "d:f:ho:qr:R:s:t:")) != -1) {
        if (c == 'd')
            parse_ok = renderer_config.set_max_distance(optarg);
        else if (c == 'f')
            parse_ok = renderer_config.set_field_of_vision(optarg);
        else if (c == 'h') {
            help_message();
            return 0;
        }
        else if (c == 'o')
            png_file = std::string(optarg);
        else if (c == 'q')
            quiet_flag = true;
        else if (c == 'r')
            parse_ok = renderer_config.set_resolution(optarg);
        else if (c == 'R')
            parse_ok = renderer_config.set_max_ray_depth(optarg);
        else if (c == 's')
            parse_ok = renderer_config.set_shadow_bias(optarg);
        else if (c == 't')
            parse_ok = renderer_config.set_num_threads(optarg);
        else if (c == '?') {
            std::cerr << "option requires argument or unknown option" << std::endl;
            return 1;
        }
        else {
            // some other error?
            return 1;
        }
        if (!parse_ok) {
            std::cerr << "error parsing option" << std::endl;
            return 1;
        }
    }

    for (int i = optind; i < argc; i++) {
        toml_files.push_back(std::string(argv[i]));
    }
    if (toml_files.empty()) {
        std::cerr << "missing toml file" << std::endl;
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
