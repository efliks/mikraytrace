#include <cstring>

#include "png.hpp"
#include "texture.h"


namespace mrtp {

static const double kRealToByte = 255;
static const double kByteToReal = 1 / kRealToByte;


TextureSharedState::TextureSharedState(const std::string& texture_filename) {
    png::image<png::rgb_pixel> image(texture_filename.c_str());

    texture_width_ = image.get_width();
    texture_heigth_ = image.get_height();

    texture_data_.reserve(texture_width_ * texture_heigth_);

    for (unsigned int i = 0; i < texture_heigth_; i++) {
        png::rgb_pixel *in = &image[i][0];
        for (unsigned j = 0; j < texture_heigth_; j++, in++) {
            MyPixel pixel{in->red, in->green, in->blue};
            texture_data_.push_back(pixel);
        }
    }
}


MyPixel TextureSharedState::pick_pickel(double frac_x, double frac_y, double scale_coeff) const {
    unsigned int u = (static_cast<unsigned int>(
                          frac_x * texture_width_ * scale_coeff)) % texture_width_;

    unsigned int v = (static_cast<unsigned int>(
                          frac_y * texture_heigth_ * scale_coeff)) % texture_heigth_;

    return texture_data_[u + v * texture_width_];
}


MyTexture::MyTexture(const std::string& texture_filename,
                     double reflection_coeff,
                     double scale_coeff) :
    reflection_coeff_(reflection_coeff),
    scale_coeff_(scale_coeff),
    is_owner_(true) {

    shared_state_ = new TextureSharedState(texture_filename);
}


MyTexture::MyTexture(MyTexture* other,
                     double reflection_coeff,
                     double scale_coeff) :
    reflection_coeff_(reflection_coeff),
    scale_coeff_(scale_coeff),
    is_owner_(false) {

    shared_state_ = other->get_shared_state();
}


MyTexture::~MyTexture() {
    if (is_owner_) {
        delete shared_state_;
    }
}


MyPixel MyTexture::pick_pickel(double frac_x, double frac_y) const {
    return shared_state_->pick_pickel(frac_x, frac_y, scale_coeff_);
}


MyTexture* TextureFactory::get_texture(const std::string& texture_filename,
                                       double reflection_coeff,
                                       double scale_coeff) {
    if (texture_map_.find(texture_filename) == texture_map_.end()) {
        // Create new texture data
        MyTexture new_texture(texture_filename, reflection_coeff, scale_coeff);
        textures_.push_back(new_texture);
        MyTexture* texture_ptr = &textures_.back();
        texture_map_.insert(std::pair<std::string, MyTexture*>(texture_filename, texture_ptr));

        return texture_ptr;
    }

    // Reuse existing texture data
    MyTexture* owner_texture_ptr = texture_map_.at(texture_filename);
    MyTexture new_texture(owner_texture_ptr, reflection_coeff, scale_coeff);

    textures_.push_back(new_texture);
    MyTexture* texture_ptr = &textures_.back();

    return texture_ptr;
}



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
