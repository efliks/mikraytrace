#include "light.h"


namespace mrtp {

Light::Light(const Eigen::Vector3d& center) : center_(center) {

}

Eigen::Vector3d Light::calculate_ray(const Eigen::Vector3d& hit) const {
  return (center_ - hit);
}


} //namespace mrtp
