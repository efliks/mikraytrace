#ifndef MAPPERS_H
#define MAPPERS_H

#include <memory>
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


std::shared_ptr<TextureMapper> create_texture_mapper(
        std::shared_ptr<ConfigTable>, ActorType, TextureFactory*);

std::shared_ptr<TextureMapper> create_dummy_mapper(std::shared_ptr<ConfigTable>,
        const std::string&, const std::string&);


}

#endif // MAPPERS_H
