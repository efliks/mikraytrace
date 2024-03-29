#ifndef ACTORS_H
#define ACTORS_H

#include <memory>
#include <Eigen/Core>

#include "common.h"
#include "mappers.h"

using Vector3d = Eigen::Vector3d;


namespace mrtp {

class ActorBase 
{
public:
    ActorBase(const StandardBasis&, std::shared_ptr<TextureMapper>);
    ActorBase() = delete;

    virtual ~ActorBase() = default;

    virtual double solve_light_ray(const Vector3d&, const Vector3d&, 
                                    double, double) const = 0;
    virtual Vector3d calculate_normal_at_hit(const Vector3d&) const = 0;
    virtual bool has_shadow() const = 0;

    MyPixel pick_pixel(const Vector3d&, const Vector3d&) const;

protected:
    StandardBasis local_basis_;
    std::shared_ptr<TextureMapper> texture_mapper_;
};


}

#endif // ACTORS_H
