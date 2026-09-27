#ifndef SPHERE_H
#define SPHERE_H

#include "config.h"
#include "actors.h"


namespace mrtp {

class SimpleSphere : public ActorBase 
{
public:
    SimpleSphere(const StandardBasis&, double, shared_ptr<TextureMapper>);

    double solve_light_ray(const Vector3d&, const Vector3d&,
            double, double) const; // override

    Vector3d calculate_normal_at_hit(const Vector3d&) const; // override
    bool has_shadow() const; // override

private:
    double radius_;
};

void create_sphere(TextureFactory*, shared_ptr<ConfigTable>, std::vector<shared_ptr<ActorBase> >*); 

}

#endif
