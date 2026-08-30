#ifndef _CAMERA_H
#define _CAMERA_H

#include "vector3.h"


namespace mrtp {

class Camera {
public:
    Camera(const Vector3d& eye, const Vector3d& lookat, double roll);

    void calculate_window(unsigned int width, unsigned int height, double perspective);

    Vector3d calculate_origin(unsigned int windowx, unsigned int windowy) const;
    Vector3d calculate_direction(const Vector3d& origin) const;

private:
    double roll_;

    Vector3d eye_;
    Vector3d lookat_;
    Vector3d wo_;
    Vector3d wh_;
    Vector3d wv_;
};

} //namespace mrtp

#endif //_CAMERA_H
