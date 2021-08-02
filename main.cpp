#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "world.h"
#include "renderer.h"
#include "texture.h"


//Default settings and limits

static const unsigned int kDefaultWidth = 640;
static const unsigned int kDefaultHeight = 480;
static const unsigned int kMinWidth = kDefaultWidth / 2;
static const unsigned int kMaxWidth = kDefaultWidth * 10;
static const unsigned int kMinHeight = kDefaultHeight / 2;
static const unsigned int kMaxHeight = kDefaultHeight * 10;

static const unsigned int kDefaultRecursionLevels = 3;
static const unsigned int kMinRecursionLevels = 0;
static const unsigned int kMaxRecursionLevels = 10;

static const unsigned int kDefaultThreads = 1;
static const unsigned int kMinThreads = 0;
static const unsigned int kMaxThreads = 64;

static const double kDefaultFOV = 93;
static const double kMinFOV = 50;
static const double kMaxFOV = 170;

static const double kDefaultDistance = 60;
static const double kDefaultShadow = 0.25;
static const double kDefaultBias = 0.001;

//Exit codes

enum ExitCode_t {exit_ok, exit_no_options, exit_unknown_option, exit_light_distance, 
                 exit_fov, exit_light_mode, exit_output_file, exit_resolution, 
                 exit_recursion_levels, exit_shadow_factor, exit_threads, exit_png, 
                 exit_toml, exit_init_world, exit_write_scene};


void help_message() {
    std::cout << R"(Usage: mrtp_cli [OPTION]... FILE...
  Options:
    -d, --light-distance     distance to darken light (def. 60)
    -f, --fov                field of vision, in degrees (def. 93)
    -h, --help               print this help screen
    -o, --output-file        output filename in PNG format
    -q, --quiet              suppress all messages, except errors
    -r, --resolution         resolution: 640x480 (def.), 1024x768, etc.
    -R, --recursion-levels   levels of recursion for reflected rays (def. 3)
    -s, --shadow-factor      shadow factor (def. 0.25)
    -t, --threads            rendering threads: 0 (auto), 1 (def.), 2, 4, etc.

Example:
  mrtp_cli -r 1620x1080 -f 110.0 -o scene2.png scene2.toml)" << std::endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        help_message();
        return exit_no_options;
    }

    unsigned int width = kDefaultWidth;
    unsigned int height = kDefaultHeight;
    unsigned int recursion = kDefaultRecursionLevels;
    unsigned int threads = kDefaultThreads;
    double fov = kDefaultFOV;
    double distance = kDefaultDistance;
    double shadow = kDefaultShadow;
    bool quiet = false;

    std::vector<std::string> toml_files;
    std::string png_file;
    
    //Begin working on options
    for (int i = 1; i < argc; ++i) {
        std::string option(argv[i]);

        if (option == "-d" || option == "--light-distance") {
            if (i + 1 >= argc) {
                std::cerr << "distance requires argument" << std::endl;
                return exit_light_distance;
            }
            std::string argument(argv[++i]);
            std::stringstream convert(argument);
            convert >> distance;
            if (!convert) {
                std::cerr << "error reading light distance" << std::endl;
                return exit_light_distance;
            }

        } else if (option == "-f" || option == "--fov") {
            if (i + 1 >= argc) {
                std::cerr << "field of vision requires argument" << std::endl;
                return exit_fov;
            }
            std::string argument(argv[++i]);
            std::stringstream convert(argument);
            convert >> fov;
            if (!convert) {
                std::cerr << "error reading field of vision" << std::endl;
                return exit_fov;
            }
            if (fov < kMinFOV || fov > kMaxFOV) {
                std::cerr << "field of vision is out of range" << std::endl;
                return exit_fov;
            }

        } else if (option == "-h" || option == "--help") {
            help_message();
            return exit_ok;

        } else if (option == "-o" || option == "--output-file") {
            if (i + 1 >= argc) {
                std::cerr << "output file requires argument" << std::endl;
                return exit_output_file;
            }
            png_file = argv[++i];
            //TODO Check for a valid filename

        } else if (option == "-q" || option == "--quiet") {
            quiet = true;

        } else if (option == "-R" || option == "--recursion-levels") {
            if (i + 1 >= argc) {
                std::cerr << "recursion requires argument" << std::endl;
                return exit_recursion_levels;
            }
            std::string argument(argv[++i]);
            std::stringstream convert(argument);
            convert >> recursion;
            if (!convert) {
                std::cerr << "error reading levels of recursion" << std::endl;
                return exit_recursion_levels;
            }
            if (recursion < kMinRecursionLevels || recursion > kMaxRecursionLevels) {
                std::cerr << "out of range recursion levels" << std::endl;
                return exit_recursion_levels;
            }

        } else if (option == "-r" || option == "--resolution") {
            if (i + 1 >= argc) {
                std::cerr << "resolution requires argument" << std::endl;
                return exit_resolution;
            }
            std::string argument(argv[++i]);
            size_t pos = argument.find('x');
            if (pos == std::string::npos) {
                pos = argument.find('X');
                if (pos == std::string::npos) {
                    std::cerr << "invalid format of resolution" << std::endl;
                    return exit_resolution;
                }
            }
            std::string left(argument.substr(0, pos));
            std::stringstream convert(left);
            convert >> width;
            if (!convert) {
                std::cerr << "unable to convert width" << std::endl;
                return exit_resolution;
            }
            if (width < kMinWidth || width > kMaxWidth) {
                std::cerr << "out of range width" << std::endl;
                return exit_resolution;
            }
            std::string right(argument.substr(pos+1, argument.length()-pos-1));
            std::stringstream convert_other(right);
            convert_other >> height;
            if (!convert_other) {
                std::cerr << "unable to convert height" << std::endl;
                return exit_resolution;
            }
            if (height < kMinHeight || height > kMaxHeight) {
                std::cerr << "out of range height" << std::endl;
                return exit_resolution;
            }

        } else if (option == "-s" || option == "--shadow-factor") {
            if (i + 1 >= argc) {
                std::cerr << "shadow factor requires argument" << std::endl;
                return exit_shadow_factor;
            }
            std::string argument(argv[++i]);
            std::stringstream convert(argument);
            convert >> shadow;
            if (!convert) {
                std::cerr << "error reading shadow factor" << std::endl;
                return exit_shadow_factor;
            }

        } else if (option == "-t" || option == "--threads") {
            if (i + 1 >= argc) {
                std::cerr << "number of threads requires argument" << std::endl;
                return exit_threads;
            }
            std::string argument(argv[++i]);
            std::stringstream convert(argument);
            convert >> threads;
            if (!convert) {
                std::cerr << "error reading number of threads" << std::endl;
                return exit_threads;
            }
            if (threads < kMinThreads || threads > kMaxThreads) {
                std::cerr << "out of range number of threads" << std::endl;
                return exit_threads;
            }

        } else {
            if (option[0] == '-') {
                std::cerr << "unrecognized option: " << option << std::endl;
                return exit_unknown_option;
            }
            std::string tmp = argv[i];
            toml_files.push_back(tmp);
        }
    }
    //Finished working on options

    if (toml_files.empty()) {
        std::cerr << "missing toml file" << std::endl;
        return exit_toml;
    }

    bool use_auto_name = (toml_files.size() > 1) || (png_file == "");
    if (use_auto_name) {
        if (png_file != "") {
            std::cerr << "multiple toml files: do not use -o/--output-file" << std::endl;
            return exit_output_file;
        }
    }

    std::vector<std::string>::iterator iter = toml_files.begin();
    std::vector<std::string>::iterator iter_end = toml_files.end();

    //Textures will be shared by all worlds
    mrtp::TextureFactory texture_factory;

    //Iterate over all input files
    for (; iter != iter_end; ++iter) {
        std::string toml_file = *iter;
        if (!quiet) { std::cout << "processing " << toml_file << std::flush; }

        auto my_world_ptr = mrtp::build_world(toml_file, &texture_factory);

        mrtp::WorldStatus_t status = mrtp::ws_ok;  // TODO Properly handle statuses

        if (status != mrtp::ws_ok) {
            if (!quiet) { std::cout << std::endl; }

            if (status == mrtp::ws_no_file) { std::cerr << "file not found" << std::endl; }
            else if (status == mrtp::ws_parse_error) { std::cerr << "error parsing file" << std::endl; }
            else if (status == mrtp::ws_no_camera) { std::cerr << "camera not found" << std::endl; }
            else if (status == mrtp::ws_no_light) { std::cerr << "light not found" << std::endl; }
            else if (status == mrtp::ws_no_actors) { std::cerr << "no actors found" << std::endl; }
            else if (status == mrtp::ws_camera_param) { std::cerr << "missing or invalid parameter in camera" << std::endl; }
            else if (status == mrtp::ws_light_param) { std::cerr << "missing or invalid parameter in light" << std::endl; }
            else if (status == mrtp::ws_plane_param) { std::cerr << "missing or invalid parameter in plane" << std::endl; }
            else if (status == mrtp::ws_sphere_param) { std::cerr << "missing or invalid parameter in sphere" << std::endl; }
            else if (status == mrtp::ws_cylinder_param) { std::cerr << "missing or invalid parameter in cylinder" << std::endl; }
            else if (status == mrtp::ws_plane_texture) { std::cerr << "cannot load texture in plane" << std::endl; }
            else if (status == mrtp::ws_sphere_texture) { std::cerr << "cannot load texture in sphere" << std::endl; }
            else if (status == mrtp::ws_cylinder_texture) { std::cerr << "cannot load texture in cylinder" << std::endl; }

            return exit_init_world;
        }

        if (use_auto_name) {
            std::string foo(toml_file);
            size_t pos = toml_file.rfind(".toml");
            if (pos != std::string::npos) { foo = toml_file.substr(0, pos); }
            png_file = foo + ".png";
        }

        mrtp::RendererConfig renderer_config{
            fov,
            distance,
            shadow,
            kDefaultBias,
            width,
            height,
            recursion
        };

        mrtp::SceneRenderer scene_renderer(my_world_ptr.get(), renderer_config);
        scene_renderer.do_render();

        mrtp::ScenePNGWriter scene_writer(&scene_renderer);
        scene_writer.write_to_file(png_file);

        std::cout << " OK" << std::endl;
    }
    
    return exit_ok;
}
