#ifndef MAPPERS_H
#define MAPPERS_H

#include <memory>
#include <Eigen/Core>

#include "config.h"
#include "common.h"
#include "texture.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;


class TextureMapper {
public:
    TextureMapper() = default;
    virtual ~TextureMapper() = default;

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
