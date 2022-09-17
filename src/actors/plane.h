#ifndef PLANE_H
#define PLANE_H

#include "config.h"
#include "actors.h"


namespace mrtp {

class SimplePlane : public ActorBase 
{
public:
    SimplePlane(const StandardBasis&, std::shared_ptr<TextureMapper>);
    SimplePlane() = delete;

    ~SimplePlane() override = default;

    double solve_light_ray(const Vector3d&, const Vector3d&, 
            double, double) const override;

    Vector3d calculate_normal_at_hit(const Vector3d&) const override;
    bool has_shadow() const override;
};


void create_plane(TextureFactory*, std::shared_ptr<ConfigTable>, 
        std::vector<std::shared_ptr<ActorBase>>*);

}

#endif
