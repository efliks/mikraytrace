#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

#include "config.h"


namespace mrtp {

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


RendererConfig::RendererConfig() {
    field_of_vision = kDefaultFOV;
    max_distance = kDefaultDistance;
    shadow_bias = kDefaultShadow;
    ray_bias = kDefaultBias;

    buffer_width = kDefaultWidth;
    buffer_height = kDefaultHeight;

    max_ray_depth = kDefaultRecursionLevels;
    num_threads = kDefaultThreads;
}


OptionParser::OptionParser(RendererConfig* renderer_config) :
    renderer_config_(renderer_config) {

}


class FieldOfVisionParser : public OptionParser {
public:
    FieldOfVisionParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~FieldOfVisionParser() override = default;

    void parse(char* s) override {
        std::string str(s);
        std::stringstream convert(str);

        convert >> renderer_config_->field_of_vision;
        is_parsed = (!convert.bad() &&
                     renderer_config_->field_of_vision >= kMinFOV &&
                     renderer_config_->field_of_vision <= kMaxFOV);
    }
};


class MaxDistanceParser : public OptionParser {
public:
    MaxDistanceParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~MaxDistanceParser() override = default;

    void parse(char* s) override {
        std::string str(s);
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

    void parse(char* s) override {
        std::string str(s);
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

    void parse(char* s) override {
        std::string str(s);
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

    void parse(char* s) override {
        std::string str(s);
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
        if (renderer_config_->buffer_width < kMinWidth ||
                renderer_config_->buffer_width > kMaxWidth) {
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
        if (renderer_config_->buffer_height < kMinHeight ||
                renderer_config_->buffer_height > kMaxHeight) {
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

    void parse(char* s) override {
        std::string str(s);
        std::stringstream convert(str);

        convert >> renderer_config_->max_ray_depth;
        is_parsed = (!convert.bad() &&
                     renderer_config_->max_ray_depth >= kMinRecursionLevels &&
                     renderer_config_->max_ray_depth <= kMaxRecursionLevels);
    }
};


class ThreadsParser : public OptionParser {
public:
    ThreadsParser(RendererConfig* renderer_config) :
        OptionParser(renderer_config) {
    }

    ~ThreadsParser() override = default;

    void parse(char* s) override {
        std::string str(s);
        std::stringstream convert(str);

        convert >> renderer_config_->num_threads;
        is_parsed = (!convert.bad() &&
                     renderer_config_->num_threads >= kMinThreads &&
                     renderer_config_->num_threads <= kMaxThreads);
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


}
