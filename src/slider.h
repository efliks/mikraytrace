#ifndef _SLIDER_H
#define _SLIDER_H

#include <memory>


namespace mrtp {

enum class ProgressSliderType
{
    DEFAULT = 1,
    DUMMY = 2
};

class ProgressSlider
{
public:
    ProgressSlider(unsigned int);
    virtual ~ProgressSlider() = default;
    virtual void tick() = 0;

protected:
    unsigned int max_tick_;
    unsigned int current_tick_;
};

std::shared_ptr<ProgressSlider> create_progress_slider(unsigned int, ProgressSliderType);

}

#endif // _SLIDER_H
