#include "light.h"

namespace mrtp {

Light::Light(const Vector3d& center)
    : center_(center)
{
}

Vector3d Light::calculate_ray(const Vector3d& hit) const
{
    return (center_ - hit);
}

} // namespace mrtp
