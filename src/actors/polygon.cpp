#include "actors/polygon.h"
#include "actors/plane.h"


namespace mrtp {

SimplePolygon::SimplePolygon(const StandardBasis& local_basis,
                             std::shared_ptr<TextureMapper> texture_mapper,
                             double xsize, double ysize)
    : ActorBase(local_basis, texture_mapper)
    , xsize_(xsize), ysize_(ysize)
{
}


double SimplePolygon::solve_light_ray(const Vector3d& O, const Vector3d& D,
                                      double min_dist, double max_dist) const
{
    SimplePlane plane(local_basis_, texture_mapper_);
    double t = plane.solve_light_ray(O, D, min_dist, max_dist);

    if (t > 0) {
        Vector3d X = (D * t) + O - local_basis_.o;
        double vx = X.dot(local_basis_.vi);
        if (vx >= -xsize_ && vx <= xsize_) {
            double vy = X.dot(local_basis_.vj);
            if (vy >= -ysize_ && vy <= ysize_) {
                return t;
            }
        }
    }

    return -1;
}


Vector3d SimplePolygon::calculate_normal_at_hit(const Vector3d& hit) const
{
    return local_basis_.vk;
}


bool SimplePolygon::has_shadow() const
{
    return true;
}


}
