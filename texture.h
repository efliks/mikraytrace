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

    MyPixel pick_pickel(double, double, double) const;

private:
    std::vector<MyPixel> texture_data_;

    unsigned int texture_width_;
    unsigned int texture_heigth_;
};


class MyTexture {
public:
    MyTexture(const std::string&, double, double);
    MyTexture(MyTexture*, double, double);
    MyTexture() = delete;
    ~MyTexture();

    TextureSharedState* get_shared_state() const;
    MyPixel pick_pickel(double, double) const;

private:
    double reflection_coeff_;
    double scale_coeff_;

    bool is_owner_;
    TextureSharedState* shared_state_;
};


class TextureFactory {
public:
    TextureFactory() = default;
    ~TextureFactory() = default;

    MyTexture* get_texture(const std::string&, double, double);

private:
    std::list<MyTexture> textures_;
    std::unordered_map<std::string, MyTexture*> texture_map_;
};


class Texture {
public:
    Texture(const std::string& texture_filename);
    Texture() = delete;
    ~Texture() = default;

    void load_texture();
    bool is_same_texture(const std::string& texture_filename);
    Pixel pick_pixel(double fracx, double fracy, double scale);

private:
    int width_;
    int height_;

    std::vector<Pixel> pixel_data_;
    std::string texture_filename_;
};


class TextureCollector {
public:
    Texture *add_texture(const std::string& texture_filename);

private:
    std::list<Texture> textures_;
};


} //namespace mrtp

#endif //_TEXTURE_H
