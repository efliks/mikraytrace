#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "pixel.h"


namespace mrtp {

struct MyPixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};


class TextureSharedState {
public:
    TextureSharedState(const std::string&);
    TextureSharedState() = delete;
    ~TextureSharedState() = default;

    MyPixel pick_pixel(double, double, double) const;
    bool is_same_texture(const std::string&) const;

private:
    std::vector<MyPixel> texture_data_;

    std::string texture_filename_;

    unsigned int texture_width_;
    unsigned int texture_heigth_;
};


class MyTexture {
public:
    MyTexture(TextureSharedState*, double, double);
    MyTexture() = delete;
    ~MyTexture() = default;

    MyPixel pick_pixel(double, double) const;

private:
    double reflection_coeff_;
    double scale_coeff_;

    TextureSharedState* shared_state_;
};


class TextureFactory {
public:
    TextureFactory() = default;
    ~TextureFactory() = default;

    MyTexture* create_texture(const std::string&, double, double);

private:
    std::list<TextureSharedState> shared_states_;
    std::list<MyTexture> textures_;
};


}  // namespace mrtp

#endif  // _TEXTURE_H
