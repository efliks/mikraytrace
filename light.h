#ifndef _LIGHT_H
#define _LIGHT_H

#include <Eigen/Core>


namespace mrtp {

class Light {
public:
    Light(const Eigen::Vector3d& center);
    ~Light() = default;

    Eigen::Vector3d calculate_ray(const Eigen::Vector3d& hit) const;

private:
    Eigen::Vector3d center_;
};


} //namespace mrtp

#endif //_LIGHT_H
