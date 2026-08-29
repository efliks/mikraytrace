#include <cstdlib>
#include <list>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "world.h"
#include "renderer.h"
#include "texture.h"
#include "writer.h"
#include "logger.h"


namespace {

void print_usage(const char* program_name)
{
    std::cout << "A simple raytracer\n\n"
              << "Usage: " << program_name << " [options] input_file...\n\n"
              << "Positional arguments:\n"
              << "  input_files        Input file(s), one or more, mandatory\n\n"
              << "Options:\n"
              << "  -h, --help         Print this help message and exit\n"
              << "  -o, --output ARG   Output file (only allowed with a single input file)\n"
              << "  -F, --format ARG   Output format, one of: png, jpg (default: png)\n"
              << "  -f, --fov ARG      Field of vision in degrees\n"
              << "  -W, --width ARG    Image width\n"
              << "  -H, --height ARG   Image height\n"
              << "  -t, --threads ARG  Rendering threads, 0 for auto\n";
}

// Parses the value following an option flag; returns false (and logs) if
// the flag is the last argument on the command line.
bool next_value(int argc, char* argv[], int& i, const std::string& flag, std::string* value)
{
    if (++i >= argc) {
        LOG_ERROR("Missing value for option " + flag);
        return false;
    }
    *value = argv[i];
    return true;
}

bool parse_double_option(const std::string& text, const std::string& flag,
                          double min_value, double max_value, double* target)
{
    char* end = 0;
    double value = std::strtod(text.c_str(), &end);
    if (*end != '\0' || value < min_value || value > max_value) {
        std::stringstream message;
        message << "Value for option " << flag << " must be a number in range ["
                << min_value << ", " << max_value << "]";
        LOG_ERROR(message.str());
        return false;
    }
    *target = value;
    return true;
}

bool parse_uint_option(const std::string& text, const std::string& flag,
                        unsigned int min_value, unsigned int max_value, unsigned int* target)
{
    char* end = 0;
    long value = std::strtol(text.c_str(), &end, 10);
    if (*end != '\0' || value < 0 ||
        static_cast<unsigned int>(value) < min_value || static_cast<unsigned int>(value) > max_value) {
        std::stringstream message;
        message << "Value for option " << flag << " must be an integer in range ["
                << min_value << ", " << max_value << "]";
        LOG_ERROR(message.str());
        return false;
    }
    *target = static_cast<unsigned int>(value);
    return true;
}

}  // namespace


int main(int argc, char* argv[])
{
    std::vector<std::string> input_files;

    std::string output_file;
    std::string output_format = "png";

    mrtp::RendererConfig config;

    // Rudimentary scan of argv
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        std::string value;

        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        else if (arg == "-o" || arg == "--output") {
            if (!next_value(argc, argv, i, arg, &value)) {
                return EXIT_FAILURE;
            }
            output_file = value;
        }
        else if (arg == "-F" || arg == "--format") {
            if (!next_value(argc, argv, i, arg, &value)) {
                return EXIT_FAILURE;
            }
            if (value != "png" && value != "jpg") {
                LOG_ERROR("Value for option " + arg + " must be one of: png, jpg");
                return EXIT_FAILURE;
            }
            output_format = value;
        }
        else if (arg == "-f" || arg == "--fov") {
            if (!next_value(argc, argv, i, arg, &value) ||
                !parse_double_option(value, arg, config.fov_min, config.fov_max, &config.fov)) {
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-W" || arg == "--width") {
            if (!next_value(argc, argv, i, arg, &value) ||
                !parse_uint_option(value, arg, config.width_min, config.width_max, &config.width)) {
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-H" || arg == "--height") {
            if (!next_value(argc, argv, i, arg, &value) ||
                !parse_uint_option(value, arg, config.height_min, config.height_max, &config.height)) {
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-t" || arg == "--threads") {
            if (!next_value(argc, argv, i, arg, &value) ||
                !parse_uint_option(value, arg, config.num_min_thread, config.num_max_thread, &config.num_thread)) {
                return EXIT_FAILURE;
            }
        }
        else if (!arg.empty() && arg[0] == '-') {
            LOG_ERROR("Unknown option: " + arg);
            return EXIT_FAILURE;
        }
        else {
            input_files.push_back(arg);
        }
    }

    if (input_files.empty()) {
        LOG_ERROR("At least one input file is required");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

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
    std::list<mrtp::TextureSharedState> texture_cache;

    mrtp::WriterType writer_type = (output_format == "png") ? mrtp::WriterType::PNG : mrtp::WriterType::JPEG;

    auto scene_renderer = mrtp::create_renderer(config);
    //FIXME pointer to renderer
    auto scene_writer = mrtp::create_writer(scene_renderer.get(), writer_type);

    // Iterate over all input files
    for (std::string& input_file : input_files) {
        LOG_INFO(std::string("Processing " + input_file + " ..."));

        mrtp::TextureFactory texture_factory(&texture_cache);
        auto world_ptr = mrtp::build_world(input_file, &texture_factory);
        if (!world_ptr) {
            return EXIT_FAILURE;
        }

        if (auto_name) {
            std::string foo(input_file);
            size_t pos = input_file.rfind(".toml");  //FIXME
            if (pos == std::string::npos) {
                pos = input_file.rfind(".txt");
            }
            if (pos != std::string::npos) {
                foo = input_file.substr(0, pos);
            }
            output_file = foo + "." + output_format;
        }

        float render_t = scene_renderer->do_render(world_ptr.get());

        std::stringstream render_time;
        render_time << "Done in " << std::setprecision(2) << render_t << "s";
        LOG_INFO(render_time.str());

        scene_writer->write_to_file(output_file);
    }
    
    return EXIT_SUCCESS;  // All done
}
