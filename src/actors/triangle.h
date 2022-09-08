#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "actors.h"


namespace mrtp {

class SimpleTriangle : public ActorBase 
{
public:
    SimpleTriangle(const StandardBasis&, const Vector3d&, const Vector3d&, 
            const Vector3d&, std::shared_ptr<TextureMapper>);
    SimpleTriangle() = delete;

    ~SimpleTriangle() override = default;

    double solve_light_ray(const Vector3d&, const Vector3d&, 
            double, double) const override;

    Vector3d calculate_normal_at_hit(const Vector3d&) const override;
    bool has_shadow() const override;

private:
    Vector3d A_;
    Vector3d B_;
    Vector3d C_;
    Vector3d TA_;
    Vector3d TB_;
    Vector3d TC_;
};


void create_triangle(TextureFactory*, std::shared_ptr<cpptoml::table>, 
        std::vector<std::shared_ptr<ActorBase>>*);

}

#endif

