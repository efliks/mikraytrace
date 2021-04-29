#ifndef _LIGHT_H
#define _LIGHT_H

#include <Eigen/Core>


namespace mrtp {

class Light {
  public:
    Light(Eigen::Vector3d *center);
    ~Light();
    Eigen::Vector3d calculate_ray(Eigen::Vector3d *hit);

  private:
    Eigen::Vector3d center_;
};

} //namespace mrtp

#endif //_LIGHT_H
