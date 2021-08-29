#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "config.h"


namespace mrtp {

RendererConfig::RendererConfig() {
    field_of_vision = 93;
    max_distance = 60;
    shadow_bias = 0.25;
    ray_bias = 0.001;

    buffer_width = 640;
    buffer_height = 480;

    max_ray_depth = 3;
    num_threads = 1;
}


class OptionParser {
public:
    OptionParser(RendererConfig* renderer_config) :
        renderer_config_(renderer_config) {
    }

    virtual ~OptionParser() = default;
    virtual void parse(const std::string&) = 0;

    bool is_parsed;

protected:
    RendererConfig* renderer_config_;
};


class FieldOfVisionParser : public OptionParser {
public:
    FieldOfVisionParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~FieldOfVisionParser() override = default;

    void parse(const std::string& str) override {
        std::stringstream convert(str);
        convert >> renderer_config_->field_of_vision;
        is_parsed = (!convert.bad() &&
                     renderer_config_->field_of_vision >= 50 &&
                     renderer_config_->field_of_vision <= 170);
    }
};


class MaxDistanceParser : public OptionParser {
public:
    MaxDistanceParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~MaxDistanceParser() override = default;

    void parse(const std::string& str) override {
        std::stringstream convert(str);
        convert >> renderer_config_->max_distance;
        is_parsed = !convert.bad();
    }
};


class ShadowBiasParser : public OptionParser {
public:
    ShadowBiasParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~ShadowBiasParser() override = default;

    void parse(const std::string& str) override {
        std::stringstream convert(str);
        convert >> renderer_config_->shadow_bias;
        is_parsed = !convert.bad();
    }
};


class RayBiasParser : public OptionParser {
public:
    RayBiasParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~RayBiasParser() override = default;

    void parse(const std::string& str) override {
        std::stringstream convert(str);
        convert >> renderer_config_->ray_bias;
        is_parsed = !convert.bad();
    }
};


class ResolutionParser : public OptionParser {
public:
    ResolutionParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~ResolutionParser() override = default;

    void parse(const std::string& str) override {
        is_parsed = false;

        size_t p = str.find('x');
        if (p == std::string::npos) {
            p = str.find('X');
            if (p == std::string::npos) {
                // invalid format of resolution
                return;
            }
        }
        std::string left(str.substr(0, p));
        std::stringstream convert(left);
        convert >> renderer_config_->buffer_width;
        if (!convert) {
            // unable to convert width
            return;
        }
        if (renderer_config_->buffer_width < 320 ||
                renderer_config_->buffer_width > 3200) {
            // width is out of range
            return;
        }
        std::string right(str.substr(p+1, str.length()-p-1));
        std::stringstream convert_other(right);
        convert_other >> renderer_config_->buffer_height;
        if (!convert_other) {
            // unable to convert heigth
            return;
        }
        if (renderer_config_->buffer_height < 240 ||
                renderer_config_->buffer_height > 2400) {
            // height is out of range
            return;
        }
        is_parsed = true;
    }
};


class RayDepthParser : public OptionParser {
public:
    RayDepthParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~RayDepthParser() override = default;

    void parse(const std::string& str) override {
        std::stringstream convert(str);
        convert >> renderer_config_->max_ray_depth;
        is_parsed = (!convert.bad() &&
                     renderer_config_->max_ray_depth >= 0 &&
                     renderer_config_->max_ray_depth <= 10);
    }
};


class ThreadsParser : public OptionParser {
public:
    ThreadsParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~ThreadsParser() override = default;

    void parse(const std::string& str) override {
        std::stringstream convert(str);
        convert >> renderer_config_->num_threads;
        is_parsed = (!convert.bad() &&
                     renderer_config_->num_threads >= 0 &&
                     renderer_config_->num_threads <= 64);
    }
};


std::shared_ptr<OptionParser> get_option_parser(int c, RendererConfig* renderer_config) {
    if (c == 'd')
        return std::shared_ptr<OptionParser>(new MaxDistanceParser(renderer_config));
    if (c == 'f')
        return std::shared_ptr<OptionParser>(new FieldOfVisionParser(renderer_config));
    if (c == 'r')
        return std::shared_ptr<OptionParser>(new ResolutionParser(renderer_config));
    if (c == 'R')
        return std::shared_ptr<OptionParser>(new RayDepthParser(renderer_config));
    if (c == 's')
        return std::shared_ptr<OptionParser>(new ShadowBiasParser(renderer_config));
    // c == 't'
    return std::shared_ptr<OptionParser>(new ThreadsParser(renderer_config));
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
            auto parser_ptr = get_option_parser(c, renderer_config);
            parser_ptr->parse(std::string(optarg));
            if (!parser_ptr->is_parsed) {
                std::cerr << "error parsing option" << std::endl;
                return false;
            }
        }
    }

    for (int i = optind; i < argc; i++) {
        input_files->push_back(std::string(argv[i]));
    }
    if (input_files->empty()) {
        std::cerr << "missing toml file" << std::endl;
        return false;
    }
    return true;
}


}
