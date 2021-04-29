#include "light.h"


namespace mrtp {

Light::Light(Eigen::Vector3d *center) : center_(*center) {}

Light::~Light() {}

Eigen::Vector3d Light::calculate_ray(Eigen::Vector3d *hit) { return (center_ - (*hit)); }

} //namespace mrtp
