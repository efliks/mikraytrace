#ifndef POLYGON_H
#define POLYGON_H

#include "actors.h"


namespace mrtp {

class SimplePolygon : public ActorBase
{
public:
    SimplePolygon(const StandardBasis&, std::shared_ptr<TextureMapper>,
                  double, double);
    SimplePolygon() = delete;

    ~SimplePolygon() override = default;

    double solve_light_ray(const Vector3d&, const Vector3d&,
            double, double) const override;

    Vector3d calculate_normal_at_hit(const Vector3d&) const override;
    bool has_shadow() const override;

private:
    double xsize_;
    double ysize_;
};


}

#endif
