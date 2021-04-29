#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <list>
#include <string>
#include <vector>
#include "pixel.h"


namespace mrtp {

class Texture {
  public:
    Texture(const char *path);
    ~Texture();
    void load_texture();
    bool check_path(const char *path);
    Pixel pick_pixel(double fracx, double fracy, double scale);

  private:
    int width_;
    int height_;
    std::vector<Pixel> data_;
    std::string spath_;
};

class TextureCollector {
  public:
    Texture *add(const char *path);

  private:
    std::list<Texture> textures_;
};


extern TextureCollector textureCollector;

} //namespace mrtp

#endif //_TEXTURE_H
