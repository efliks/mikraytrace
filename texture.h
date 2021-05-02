#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <list>
#include <string>
#include <vector>
#include "pixel.h"


namespace mrtp {

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
