#include "lodepng.h"
#include "texture.h"


namespace mrtp {

TexturePixel::TexturePixel() :
    red(0), green(0), blue(0) {
}


TexturePixel::TexturePixel(unsigned char r,
                           unsigned char g,
                           unsigned char b) :
    red(r), green(g), blue(b) {
}


TexturePixel::TexturePixel(const Vector3d& color_vec)
{
    from_vec(color_vec);
}


Vector3d TexturePixel::to_vec() const {
    return Vector3d{
        static_cast<double>(red),
        static_cast<double>(green),
        static_cast<double>(blue)
    } / 255;
}


void TexturePixel::from_vec(const Vector3d& color_vec)
{
    Vector3d v = color_vec * 255;

    red = static_cast<unsigned char>(v[0]);
    green = static_cast<unsigned char>(v[1]);
    blue = static_cast<unsigned char>(v[2]);
}


TextureSharedState::TextureSharedState(const std::string& filename)
{
    // TODO Stop on errors
    std::vector<unsigned char> buffer;
    lodepng::decode(buffer, texture_width_, texture_heigth_, filename);

    texture_data_.resize(texture_width_ * texture_heigth_);

    static_assert(sizeof(TexturePixel) == sizeof(unsigned char) * 4, "Cannot copy temporary buffer into texture data");
    std::memcpy(static_cast<void *>(texture_data_.data()), static_cast<void *>(buffer.data()), sizeof(TexturePixel) * texture_width_ * texture_heigth_);
}


/*
fracx, fracy are within a range of <0..1> and
define fractions of the x- and y-dimension
of a texture.
A reasonable scale for a 256x256 texture is 0.15.
*/
TexturePixel TextureSharedState::pick_pixel(double frac_x, double frac_y, double scale_coeff) const {
    unsigned int u = (static_cast<unsigned int>(
                          frac_x * static_cast<double>(texture_width_) * scale_coeff)) % texture_width_;
    unsigned int v = (static_cast<unsigned int>(
                          frac_y * static_cast<double>(texture_heigth_) * scale_coeff)) % texture_heigth_;

    return texture_data_[u + v * texture_width_];
}


bool TextureSharedState::is_same_texture(const std::string& texture_filename) const {
    return texture_filename_ == texture_filename;
}


MyTexture::MyTexture(TextureSharedState* shared_state,
                     double reflection_coeff,
                     double scale_coeff) :
    reflection_coeff_(reflection_coeff),
    scale_coeff_(scale_coeff),
    shared_state_(shared_state) {

}


MyPixel MyTexture::pick_pixel(double frac_x, double frac_y) const {
    TexturePixel pixel = shared_state_->pick_pixel(frac_x, frac_y, scale_coeff_);

    return MyPixel{pixel, reflection_coeff_};
}


MyTexture* TextureFactory::create_texture(const std::string& texture_filename,
                                          double reflection_coeff,
                                          double scale_coeff) {
    for (auto& shared_state : shared_states_) {
        if (shared_state.is_same_texture(texture_filename)) {
            MyTexture new_texture(&shared_state, reflection_coeff, scale_coeff);
            textures_.push_back(new_texture);
            return &textures_.back();
        }
    }

    TextureSharedState new_shared_state(texture_filename);
    shared_states_.push_back(new_shared_state);

    MyTexture new_texture(&shared_states_.back(), reflection_coeff, scale_coeff);
    textures_.push_back(new_texture);
    return &textures_.back();
}


}  // namespace mrtp
