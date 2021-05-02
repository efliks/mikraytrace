#include <cstring>

#include "png.hpp"
#include "texture.h"


namespace mrtp {

static const double kRealToByte = 255;
static const double kByteToReal = 1 / kRealToByte;


Texture::Texture(const std::string& texture_filename) :
  texture_filename_(texture_filename) {

}


/*
fracx, fracy are within a range of <0..1> and
define fractions of the x- and y-dimension
of a texture.
A reasonable scale for a 256x256 texture is 0.15.
*/
Pixel Texture::pick_pixel(double fracx, double fracy, double scale) {
    unsigned u = (static_cast<unsigned>(fracx * width_ * scale)) % width_;
    unsigned v = (static_cast<unsigned>(fracy * height_ * scale)) % height_;

    return pixel_data_[u + v * width_];
}


bool Texture::is_same_texture(const std::string& texture_filename) {
    return texture_filename_ == texture_filename;
}


void Texture::load_texture() {
    png::image<png::rgb_pixel> image(texture_filename_.c_str());

    width_ = image.get_width();
    height_ = image.get_height();
    pixel_data_.reserve(width_ * height_);

    for (int i = 0; i < height_; i++) {
        png::rgb_pixel *in = &image[i][0];

        for (int j = 0; j < width_; j++, in++) {
            Pixel out(
                static_cast<double>(in->red),
                static_cast<double>(in->green),
                static_cast<double>(in->blue)
            );
            out *= kByteToReal;
            pixel_data_.push_back(out);
        }
    }
}

/*
Adds a texture to a texture collector or reuses
one that already exists in the memory.
Returns a pointer to the texture.
*/
Texture *TextureCollector::add_texture(const std::string& texture_filename) {
    std::list<Texture>::iterator iter = textures_.begin();
    std::list<Texture>::iterator iter_end = textures_.end();

    for (; iter != iter_end; ++iter) {
        Texture *texture = &(*iter);
        if (texture->is_same_texture(texture_filename)) {
            return texture;
        }
    }

    Texture texture(texture_filename);
    textures_.push_back(texture);

    Texture *last = &textures_.back();
    last->load_texture();

    return last;
}

} //namespace mrtp
