#include "light.h"


namespace mrtp {

Light::Light(Eigen::Vector3f *center) : center_(*center) {}

Light::~Light() {}

Eigen::Vector3f Light::calculate_ray(Eigen::Vector3f *hit) { return (center_ - (*hit)); }

} //namespace mrtp
