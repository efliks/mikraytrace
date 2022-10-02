#include "png.hpp"

#include "writer.h"


namespace mrtp {

SceneWriterBase::SceneWriterBase(SceneRendererBase* renderer) 
    : scene_renderer_(renderer) 
{
}


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
    return std::shared_ptr<SceneWriterBase>(new SceneWriterPNG(renderer));
}


}
