#include <cmath>
#include "camera.h"

static double pi() { return std::atan(1.0) * 4; }


namespace mrtp {

Camera::Camera(const Vector3d& eye, const Vector3d& lookat, double roll) :
  roll_(roll), eye_(eye), lookat_(lookat) {

}


void Camera::calculate_window(unsigned int width,
                              unsigned int height,
                              double perspective) {
    // i is a vector between the camera and the center of the window
    Vector3d i = lookat_ - eye_;
    i *= (1 / i.norm());

    Vector3d k(0, 0, 1);

    Vector3d j = i.cross(k);
    j *= (1 / j.norm());

    k = j.cross(i);
    k *= (1 / k.norm());

    // Rotate camera around the i axis
    double roll = roll_ * pi() / 180;
    double sina = std::sin(roll);
    double cosa = std::cos(roll);

    Vector3d jp = cosa * j + sina * k;
    Vector3d kp = -sina * j + cosa * k;

    j = jp;
    k = kp;

    // Calculate the central point of the window
    Vector3d center = eye_ + perspective * i;

    // Find three corners of the window
    wo_ = center - 0.5 * j + 0.5 * k;

    Vector3d h = wo_ + j;
    Vector3d v = wo_ - k;

    // Find vectors spanning the window
    wh_ = 1 / static_cast<double>(width) * (h - wo_);
    wv_ = 1 / static_cast<double>(height) * (v - wo_);
}


Vector3d Camera::calculate_origin(unsigned int windowx,
                                  unsigned int windowy) const {
    return wo_ + static_cast<double>(windowx) * wh_ + static_cast<double>(windowy) * wv_;
}


Vector3d Camera::calculate_direction(const Vector3d& origin) const {
    Vector3d direction = origin - eye_;
    return direction * (1 / direction.norm());
}

} //namespace mrtp
