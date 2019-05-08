/* File      : texture.hpp
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <list>
#include <vector>
#include <string>

#include "pixel.hpp"


class Texture {
public:
    Texture (const char *path);
    ~Texture () {}
    void load_texture ();
    bool check_path (const char *path);
    Pixel pick_pixel (float fracx, float fracy, float scale);

private:
    int width_;
    int height_;
    std::vector<Pixel> data_;
    std::string spath_;
};

class TextureCollector {
public:
    Texture *add (const char *path);

private:
    std::list<Texture> textures_;
};


extern TextureCollector textureCollector;

#endif  //!_TEXTURE_H
