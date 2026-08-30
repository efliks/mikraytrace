#ifndef _LIGHT_H
#define _LIGHT_H

#include "vector3.h"


namespace mrtp {

class Light {
public:
    Light(const Vector3d& center);
    ~Light() = default;

    Vector3d calculate_ray(const Vector3d& hit) const;

private:
    Vector3d center_;
};


} //namespace mrtp

#endif //_LIGHT_H
