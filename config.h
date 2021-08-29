#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>


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


bool process_command_line(int, char**, RendererConfig*,
                          std::vector<std::string>*, std::string*, bool*);


}

#endif // CONFIG_H
