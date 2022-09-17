#ifndef CYLINDER_H
#define CYLINDER_H

#include "actors.h"
#include "config.h"


namespace mrtp {

class SimpleCylinder : public ActorBase 
{
public:
    SimpleCylinder(const StandardBasis&, 
            double, double, std::shared_ptr<TextureMapper>);

    SimpleCylinder() = delete;
    ~SimpleCylinder() override = default;

    double solve_light_ray(const Vector3d&, const Vector3d&, 
            double, double) const override;

    Vector3d calculate_normal_at_hit(const Vector3d&) const override;
    bool has_shadow() const override;

private:
    double radius_;
    double length_;
};


void create_cylinder(TextureFactory*, std::shared_ptr<BaseTable>, 
        std::vector<std::shared_ptr<ActorBase>>*);

}

#endif
