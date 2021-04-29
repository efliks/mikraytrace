#include <Eigen/Geometry>
#include <cmath>
#include "camera.h"


namespace mrtp {

Camera::Camera(Eigen::Vector3d *eye, Eigen::Vector3d *lookat, double roll) : eye_(*eye), lookat_(*lookat), roll_(roll) {}

Camera::~Camera() {}

void Camera::calculate_window(int width, int height, double perspective) {
    // i is a vector between the camera and the center of the window
    Eigen::Vector3d i = lookat_ - eye_;
    i *= (1 / i.norm());

    Eigen::Vector3d k;
    k << 0, 0, 1;

    Eigen::Vector3d j = i.cross(k);
    j *= (1 / j.norm());

    k = j.cross(i);
    k *= (1 / k.norm());

    // Rotate camera around the i axis
    double roll = roll_ * M_PI / 180;
    double sina = std::sin(roll);
    double cosa = std::cos(roll);

    Eigen::Vector3d jp = cosa * j + sina * k;
    Eigen::Vector3d kp = -sina * j + cosa * k;

    j = jp;
    k = kp;

    // Calculate the central point of the window
    Eigen::Vector3d center = eye_ + perspective * i;

    // Find three corners of the window
    wo_ = center - 0.5 * j + 0.5 * k;

    Eigen::Vector3d h = wo_ + j;
    Eigen::Vector3d v = wo_ - k;

    // Find vectors spanning the window
    wh_ = 1 / static_cast<double>(width) * (h - wo_);
    wv_ = 1 / static_cast<double>(height) * (v - wo_);
}

Eigen::Vector3d Camera::calculate_origin(int windowx, int windowy) {
    return (wo_ + static_cast<double>(windowx) * wh_ + static_cast<double>(windowy) * wv_);
}

Eigen::Vector3d Camera::calculate_direction(Eigen::Vector3d *origin) {
    Eigen::Vector3d direction = (*origin) - eye_;
    return (direction * (1 / direction.norm()));
}

} //namespace mrtp
