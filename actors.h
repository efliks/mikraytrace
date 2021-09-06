#ifndef ACTORS_H
#define ACTORS_H

#include <memory>
#include <Eigen/Core>
#include "cpptoml.h"
#include "pixel.h"
#include "texture.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;


class StandardBasis {
public:
    StandardBasis(const Vector3d&, const Vector3d&,
                    const Vector3d&, const Vector3d&);
    StandardBasis();
    ~StandardBasis() = default;

    Vector3d o;
    Vector3d vi;
    Vector3d vj;
    Vector3d vk;
};


class TextureMapper {
public:
    TextureMapper() = default;
    virtual ~TextureMapper() = default;

    virtual MyPixel pick_pixel(
            const Vector3d&, const Vector3d&, const StandardBasis&) const = 0;
};


class ActorBase {
public:
    ActorBase(const StandardBasis&, std::shared_ptr<TextureMapper>);
    ActorBase() = delete;
    virtual ~ActorBase() = default;

    virtual double solve_light_ray(const Vector3d&, const Vector3d&, double,
                                   double) const = 0;
    virtual MyPixel pick_pixel(const Vector3d&, const Vector3d&) const = 0;
    virtual Vector3d calculate_normal_at_hit(const Vector3d&) const = 0;
    virtual bool has_shadow() const = 0;

protected:
    StandardBasis local_basis_;
    std::shared_ptr<TextureMapper> texture_mapper_;
};


class TomlActorFactory {
public:
    TomlActorFactory(TextureFactory*);
    TomlActorFactory() = delete;
    ~TomlActorFactory() = default;

    std::shared_ptr<ActorBase> create_plane(std::shared_ptr<cpptoml::table>);
    std::shared_ptr<ActorBase> create_sphere(std::shared_ptr<cpptoml::table>);
    std::shared_ptr<ActorBase> create_cylinder(std::shared_ptr<cpptoml::table>);

private:
    TextureFactory* texture_factory_;
};


}

#endif // ACTORS_H
