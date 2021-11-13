#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <easylogging++.h>

#include "world.h"
#include "renderer.h"
#include "texture.h"

INITIALIZE_EASYLOGGINGPP


using RendererConfig = mrtp::RendererConfig;


bool parse_field_of_vision(const std::string& s,
                           RendererConfig* config) {
    std::stringstream convert(s);
    convert >> config->field_of_vision;

    bool is_parsed;
    if (!(is_parsed = !convert.bad())) {
        LOG(ERROR) << "Error parsing field of vision";
        return is_parsed;
    }
    if (!(is_parsed = config->field_of_vision >= 50 &&
          config->field_of_vision <= 170)) {
        LOG(ERROR) << "Field of vision is out of range";
    }

    return is_parsed;
}


bool parse_max_distance(const std::string& s,
                        RendererConfig* config) {
    std::stringstream convert(s);
    convert >> config->max_distance;

    bool is_parsed;
    if (!(is_parsed = !convert.bad()))
        LOG(ERROR) << "Error parsing maximum distance";

    return is_parsed;
}


bool parse_shadow_bias(const std::string& s,
                       RendererConfig* config) {
    std::stringstream convert(s);
    convert >> config->shadow_bias;

    bool is_parsed;
    if (!(is_parsed = !convert.bad()))
        LOG(ERROR) << "Error parsing shadow bias";

    return is_parsed;
}


bool parse_ray_depth(const std::string& s,
                     RendererConfig* config) {
    std::stringstream convert(s);
    convert >> config->max_ray_depth;

    bool is_parsed;
    if (!(is_parsed = !convert.bad()))
        LOG(ERROR) << "Error parsing maximum ray depth";

    return is_parsed;
}


bool parse_threads(const std::string& s,
                   RendererConfig* config) {
    std::stringstream convert(s);
    convert >> config->num_threads;

    bool is_parsed;
    if (!(is_parsed = !convert.bad())) {
        LOG(ERROR) << "Error parsing number of threads";
        return is_parsed;
    }
    if (!(is_parsed = config->num_threads >= 0 &&
          config->num_threads <= 64)) {
        LOG(ERROR) << "Number of threads is out of range";
    }

    return is_parsed;
}


bool parse_resolution(const std::string& str,
                      RendererConfig* config) {
    bool is_parsed = true;

    size_t p = str.find('x');
    if (p == std::string::npos) {
        p = str.find('X');
        if (p == std::string::npos) {
            is_parsed = false;
        }
    }
    if (!is_parsed) {
        LOG(ERROR) << "Invalid resolution format";
        return is_parsed;
    }

    std::string left(str.substr(0, p));
    std::stringstream convert(left);
    convert >> config->buffer_width;

    if (!(is_parsed = !convert.bad())) {
        LOG(ERROR) << "Unable to convert resolution width";
        return is_parsed;
    }
    if (!(is_parsed = config->buffer_width >= 320 &&
          config->buffer_width <= 3200)) {
        LOG(ERROR) << "Resolution width is out of range";
        return is_parsed;
    }

    std::string right(str.substr(p+1, str.length()-p-1));
    std::stringstream convert_other(right);
    convert_other >> config->buffer_height;

    if (!(is_parsed = !convert_other.bad())) {
        LOG(ERROR) << "Unable to convert resolution height";
        return is_parsed;
    }
    if (!(is_parsed = config->buffer_height >= 240 &&
          config->buffer_height <= 2400)) {
        LOG(ERROR) << "Resolution height is out of range";
    }

    return is_parsed;
}


bool apply_option_parser(int c,
                         const std::string& opt_arg,
                         RendererConfig* renderer_config) {
    if (c == 'd')
        return parse_max_distance(opt_arg, renderer_config);
    if (c == 'f')
        return parse_field_of_vision(opt_arg, renderer_config);
    if (c == 'r')
        return parse_resolution(opt_arg, renderer_config);
    if (c == 'R')
        return parse_ray_depth(opt_arg, renderer_config);
    if (c == 's')
        return parse_shadow_bias(opt_arg, renderer_config);
    // c == 't'
    return parse_threads(opt_arg, renderer_config);
}


void display_help() {
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


bool process_command_line(int argc,
                          char** argv,
                          RendererConfig* renderer_config,
                          std::vector<std::string>* input_files,
                          std::string* output_file,
                          bool* quiet_mode) {
    if (argc < 2) {
        display_help();
        return false;
    }

    int c;
    *quiet_mode = false;

    while ((c = getopt(argc, argv, "d:f:ho:qr:R:s:t:")) != -1) {
        if (c == 'h') {
            display_help();
            return false;
        }
        else if (c == 'o') {
            *output_file = std::string(optarg);
        }
        else if (c == 'q') {
            *quiet_mode = true;
        }
        else if (c == '?') {
            return false;
        }
        else {
            if (!apply_option_parser(c, std::string(optarg), renderer_config)) {
                return false;
            }
        }
    }

    for (int i = optind; i < argc; i++) {
        input_files->push_back(std::string(argv[i]));
    }
    if (input_files->empty()) {
        LOG(ERROR) << "Missing toml file";
        return false;
    }
    return true;
}


int main(int argc, char** argv) {
    bool quiet_flag = false;
    std::string png_file;
    std::vector<std::string> toml_files;
    mrtp::RendererConfig renderer_config;

    if (!(process_command_line(
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
            LOG(ERROR) << "Option -o not allowed with multiple toml files";
            return 1;
        }
    }

    // Textures will be shared by all worlds
    mrtp::TextureFactory texture_factory;

    // Iterate over all input files
    for (auto toml_file : toml_files) {
        LOG(INFO) << "Processing " << toml_file << " ...";

        auto world_ptr = mrtp::build_world(toml_file, &texture_factory);
        if (!world_ptr)
            return 2;

        if (use_auto_name) {
            std::string foo(toml_file);
            size_t pos = toml_file.rfind(".toml");
            if (pos != std::string::npos) {
                foo = toml_file.substr(0, pos);
            }
            png_file = foo + ".png";
        }

        if (renderer_config.num_threads == 1) {
            mrtp::SceneRenderer scene_renderer(world_ptr.get(), renderer_config);
            mrtp::ScenePNGWriter scene_writer(&scene_renderer);

            scene_renderer.do_render();
            scene_writer.write_to_file(png_file);
        }
        else {
            mrtp::ParallelSceneRenderer scene_renderer(world_ptr.get(), renderer_config, renderer_config.num_threads);
            mrtp::ScenePNGWriter scene_writer(&scene_renderer);

            scene_renderer.do_render();
            scene_writer.write_to_file(png_file);
        }
    }
    
    return 0;  // All done
}
