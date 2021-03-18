/* File      : light.cpp
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#include "light.h"


namespace mrtp {

Light::Light(Eigen::Vector3f *center) : center_(*center) {}

Light::~Light() {}

Eigen::Vector3f Light::calculate_ray(Eigen::Vector3f *hit) { return (center_ - (*hit)); }

} //namespace mrtp
