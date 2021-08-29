#ifndef CONFIG_H
#define CONFIG_H

#include <memory>


namespace mrtp {

class RendererConfig {
public:
    RendererConfig();
    ~RendererConfig() = default;

    double field_of_vision;
    double max_distance;
    double shadow_bias;
    double ray_bias;

    unsigned int buffer_width;
    unsigned int buffer_height;

    unsigned int max_ray_depth;
    unsigned int num_threads;
};


class OptionParser {
public:
    OptionParser(RendererConfig*);
    virtual ~OptionParser() = default;
    virtual void parse(const std::string&) = 0;

    bool is_parsed;

protected:
    RendererConfig* renderer_config_;
};


std::shared_ptr<OptionParser> get_option_parser(int, RendererConfig*);


}

#endif // CONFIG_H
