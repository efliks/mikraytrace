#include <vector>
#include <fstream>
#include <iostream>

#include "lodepng.h"
#include "toojpeg.h"

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

    void write_to_file(const std::string& filename) // override
    {
        std::vector<unsigned char> buffer;
        buffer.reserve(3 * scene_renderer_->framebuffer_.size());

        TexturePixel* in = &scene_renderer_->framebuffer_[0];

        for (unsigned int i = 0; i < scene_renderer_->config_.height; i++) {
            for (unsigned int j = 0; j < scene_renderer_->config_.width; j++, in++) {
                buffer.push_back(in->red);
                buffer.push_back(in->green);
                buffer.push_back(in->blue);
            }
        }

        fileout.open(filename.c_str(), std::ios::out | std::ios::binary);
        TooJpeg::writeJpeg(write_byte, &buffer[0], scene_renderer_->config_.width, scene_renderer_->config_.height, true, quality_);

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

    void write_to_file(const std::string& filename) // override
    {
        lodepng::State state;

        state.info_raw.colortype = LCT_RGBA;
        state.info_raw.bitdepth = 8;

        state.info_png.color.colortype = LCT_RGB;
        state.info_png.color.bitdepth = 8;
        state.encoder.auto_convert = 0;

        std::vector<unsigned char> buffer;
        unsigned int error = lodepng::encode(buffer, static_cast<unsigned char *>(static_cast<void *>(&scene_renderer_->framebuffer_[0])), scene_renderer_->config_.width, scene_renderer_->config_.height, state);

        if (!error) {
            std::cout << "INFO: Writing scene image " << filename.c_str() << " ..." << std::endl;
            error = lodepng::save_file(buffer, filename);
        }

        if (error) {
            std::cerr << "ERROR: Error writing scene image: " << lodepng_error_text(error) << std::endl;
        }
    }
};


shared_ptr<SceneWriterBase> create_writer(SceneRendererBase* renderer, WriterType type)
{
    if (type == WriterType_PNG) {
        return shared_ptr<SceneWriterBase>(new SceneWriterPNG(renderer));
    }

    //TODO Configure quality
    return shared_ptr<SceneWriterBase>(new SceneWriterJPEG(renderer, 90));
}


}
