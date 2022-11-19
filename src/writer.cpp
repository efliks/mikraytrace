#include <vector>
#include <fstream>

#include "lodepng.h"
#include "toojpeg.h"

#include "writer.h"
#include "logger.h"


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

        TexturePixel* in = &scene_renderer_->framebuffer_[0];

        for (unsigned int i = 0; i < scene_renderer_->config_.height; i++) {
            for (unsigned int j = 0; j < scene_renderer_->config_.width; j++, in++) {
                buffer.push_back(in->red);
                buffer.push_back(in->green);
                buffer.push_back(in->blue);
            }
        }

        fileout.open(filename, std::ios_base::out | std::ios_base::binary);
        TooJpeg::writeJpeg(write_byte, buffer.data(), scene_renderer_->config_.width, scene_renderer_->config_.height, true, quality_);

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
        lodepng::State state;

        state.info_raw.colortype = LCT_RGBA;
        state.info_raw.bitdepth = 8;

        state.info_png.color.colortype = LCT_RGB;
        state.info_png.color.bitdepth = 8;
        state.encoder.auto_convert = 0;

        std::vector<unsigned char> buffer;
        static_assert (sizeof(TexturePixel) == 4, "Wrong size of TexturePixel");
        unsigned int error = lodepng::encode(buffer, static_cast<unsigned char *>(static_cast<void *>(scene_renderer_->framebuffer_.data())), scene_renderer_->config_.width, scene_renderer_->config_.height, state);

        if (!error) {
            LOG_INFO(std::string("Writing scene image " + filename + " ..."));
            error = lodepng::save_file(buffer, filename);
        }

        if (error) {
            std::string lodepng_error(lodepng_error_text(error));
            LOG_ERROR(std::string("Error writing scene image: " + lodepng_error));
        }
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
