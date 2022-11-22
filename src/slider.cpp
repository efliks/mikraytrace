#include <iostream>
#include <string>
#include <cmath>

#include "slider.h"

extern "C" {
#if (defined (LINUX) || defined (__linux__))

#include <sys/ioctl.h> // ioctl() and TIOCGWINSZ
#include <unistd.h> // STDOUT_FILENO

unsigned int get_columns()
{
    struct winsize win_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);
    return static_cast<unsigned int>(win_size.ws_col);
}
#else  // probably MS-DOS
unsigned int get_columns()
{
    return 80;
}
#endif

}


namespace mrtp {

ProgressSlider::ProgressSlider(unsigned int max_tick)
    : max_tick_(max_tick), current_tick_(0)
{
}


class DefaultProgressSlider : public ProgressSlider
{
public:
    DefaultProgressSlider(unsigned int max_tick)
        : ProgressSlider(max_tick)
    {
        num_col_ = get_columns() / 2;
    }

    ~DefaultProgressSlider() override = default;

    void tick() override
    {
        double progress = static_cast<double>(current_tick_) / static_cast<double>(max_tick_);

        unsigned int num_ticks = static_cast<unsigned int>(std::round(progress * num_col_));

        std::string bar(num_ticks, 'x');
        std::string fill(num_col_ - num_ticks, ' ');
        std::cout << "<" << bar << fill << ">\r" << std::flush;

        if (++current_tick_ == max_tick_) {
            current_tick_ = 0;
            std::cout << "\r\e[K" << std::flush;  // clear progress slider
        }
    }

private:
    unsigned int num_col_;
};


class DummyProgressSlider : public ProgressSlider
{
public:
    DummyProgressSlider(unsigned int max_tick)
        : ProgressSlider(max_tick)
    {
    }

    ~DummyProgressSlider() override = default;

    void tick() override
    {
    }
};


std::shared_ptr<ProgressSlider> create_progress_slider(unsigned int max_tick,
    ProgressSliderType slider_type)
{
    if (slider_type == ProgressSliderType::DUMMY) {
        return std::shared_ptr<ProgressSlider>(new DummyProgressSlider(max_tick));
    }

    return std::shared_ptr<ProgressSlider>(new DefaultProgressSlider(max_tick));
}

}
