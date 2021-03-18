/* File      : light.h
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#ifndef _LIGHT_H
#define _LIGHT_H

#include <Eigen/Core>


namespace mrtp {

class Light {
  public:
    Light(Eigen::Vector3f *center);
    ~Light();
    Eigen::Vector3f calculate_ray(Eigen::Vector3f *hit);

  private:
    Eigen::Vector3f center_;
};

} //namespace mrtp

#endif //_LIGHT_H
