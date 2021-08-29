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

    width = kDefaultWidth;
    height = kDefaultHeight;

    max_ray_depth = kDefaultRecursionLevels;
    num_threads = kDefaultThreads;
}


OptionParser::OptionParser(RendererConfig* renderer_config) :
    is_parsed(false), renderer_config_(renderer_config) {

}


FieldOfVisionParser::FieldOfVisionParser(RendererConfig* renderer_config) :
    OptionParser(renderer_config) {

}


MaxDistanceParser::MaxDistanceParser(RendererConfig* renderer_config) :
    OptionParser(renderer_config) {

}


ShadowBiasParser::ShadowBiasParser(RendererConfig* renderer_config) :
    OptionParser(renderer_config) {

}

RayBiasParser::RayBiasParser(RendererConfig* renderer_config) :
    OptionParser(renderer_config) {

}


ResolutionParser::ResolutionParser(RendererConfig* renderer_config) :
    OptionParser(renderer_config) {

}


RayDepthParser::RayDepthParser(RendererConfig* renderer_config) :
    OptionParser(renderer_config) {

}


ThreadsParser::ThreadsParser(RendererConfig* renderer_config) :
    OptionParser(renderer_config) {

}


void FieldOfVisionParser::parse(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> renderer_config_->field_of_vision;
    is_parsed = (!convert.bad() &&
                 renderer_config_->field_of_vision >= kMinFOV &&
                 renderer_config_->field_of_vision <= kMaxFOV);
}


void MaxDistanceParser::parse(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> renderer_config_->max_distance;
    is_parsed = !convert.bad();
}


void ShadowBiasParser::parse(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> renderer_config_->shadow_bias;
    is_parsed = !convert.bad();
}


void RayBiasParser::parse(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> renderer_config_->ray_bias;
    is_parsed = !convert.bad();
}


void ResolutionParser::parse(char* s) {
    std::string str(s);

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
    convert >> renderer_config_->width;
    if (!convert) {
        // unable to convert width
        return;
    }
    if (renderer_config_->width < kMinWidth ||
            renderer_config_->width > kMaxWidth) {
        // width is out of range
        return;
    }
    std::string right(str.substr(p+1, str.length()-p-1));
    std::stringstream convert_other(right);
    convert_other >> renderer_config_->height;
    if (!convert_other) {
        // unable to convert heigth
        return;
    }
    if (renderer_config_->height < kMinHeight ||
            renderer_config_->height > kMaxHeight) {
        // height is out of range
        return;
    }
    is_parsed = true;
}


void RayDepthParser::parse(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> renderer_config_->max_ray_depth;
    is_parsed = (!convert.bad() &&
                 renderer_config_->max_ray_depth >= kMinRecursionLevels &&
                 renderer_config_->max_ray_depth <= kMaxRecursionLevels);
}


void ThreadsParser::parse(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> renderer_config_->num_threads;
    is_parsed = (!convert.bad() &&
                 renderer_config_->num_threads >= kMinThreads &&
                 renderer_config_->num_threads <= kMaxThreads);
}


}
