#ifndef MAPPERS_H
#define MAPPERS_H

#include "shrptr.h"
#include "vector3.h"

#include "config.h"
#include "common.h"
#include "texture.h"


namespace mrtp {

class TextureMapper {
public:
    virtual ~TextureMapper() {}

    virtual MyPixel pick_pixel(const Vector3d&,
                               const Vector3d&,
                               const StandardBasis&
                               ) const = 0;
};


shared_ptr<TextureMapper> create_texture_mapper(
        shared_ptr<ConfigTable>, ActorType, TextureFactory*);

shared_ptr<TextureMapper> create_dummy_mapper(shared_ptr<ConfigTable>,
        const std::string&, const std::string&);


}

#endif // MAPPERS_H
