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
