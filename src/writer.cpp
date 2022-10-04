#include <vector>
#include <fstream>

#include "png.hpp"
#include "toojpeg.h"

#include "pixel.h"
#include "writer.h"


namespace mrtp {

SceneWriterBase::SceneWriterBase(SceneRendererBase* renderer) 
    : scene_renderer_(renderer) 
{
}


//FIXME
static std::ofstream fileout;

static void write_byte(unsigned char b)
{
    fileout << b;
}


class SceneWriterJPEG : public SceneWriterBase
{
public:
    SceneWriterJPEG(SceneRendererBase* renderer, unsigned char quality)
        : SceneWriterBase(renderer)
        , quality_(quality)
    {
    }

    ~SceneWriterJPEG() override = default;

    void write_to_file(const std::string& filename) override
    {
        std::vector<unsigned char> buffer;
        buffer.reserve(3 * scene_renderer_->framebuffer_.size());

        Pixel* in = &scene_renderer_->framebuffer_[0];

        for (unsigned int i = 0; i < scene_renderer_->config_.buffer_height; i++) {
            for (unsigned int j = 0; j < scene_renderer_->config_.buffer_width; j++, in++) {
                Pixel bytes = 255 * (*in);
                buffer.push_back(static_cast<unsigned char>(bytes[0]));
                buffer.push_back(static_cast<unsigned char>(bytes[1]));
                buffer.push_back(static_cast<unsigned char>(bytes[2]));
            }
        }

        fileout.open(filename, std::ios_base::out | std::ios_base::binary);
        TooJpeg::writeJpeg(write_byte, buffer.data(), scene_renderer_->config_.buffer_width, scene_renderer_->config_.buffer_height, true, quality_);

        fileout.close();
    }

private:
    unsigned char quality_;
};


class SceneWriterPNG : public SceneWriterBase
{
public:
    SceneWriterPNG(SceneRendererBase* renderer)
        : SceneWriterBase(renderer)
    {
    }

    ~SceneWriterPNG() override = default;

    void write_to_file(const std::string& filename) override
    {
        png::image<png::rgb_pixel> image(
                    scene_renderer_->config_.buffer_width, scene_renderer_->config_.buffer_height);

        Pixel* in = &scene_renderer_->framebuffer_[0];

        for (unsigned int i = 0; i < scene_renderer_->config_.buffer_height; i++) {
            png::rgb_pixel* out = &image[i][0];
            for (unsigned int j = 0; j < scene_renderer_->config_.buffer_width; j++, in++, out++) {
                Pixel bytes = 255 * (*in);
                out->red = static_cast<unsigned char>(bytes[0]);
                out->green = static_cast<unsigned char>(bytes[1]);
                out->blue = static_cast<unsigned char>(bytes[2]);
            }
        }

        image.write(filename.c_str());
    }
};


std::shared_ptr<SceneWriterBase> create_writer(SceneRendererBase* renderer, WriterType type)
{
    if (type == WriterType::PNG) {
        return std::shared_ptr<SceneWriterBase>(new SceneWriterPNG(renderer));
    }

    //TODO Configure quality
    return std::shared_ptr<SceneWriterBase>(new SceneWriterJPEG(renderer, 90));
}


}
