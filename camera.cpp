/* File      : camera.cpp
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#include <Eigen/Geometry>
#include <cmath>

#include "camera.h"


namespace mrtp {

Camera::Camera(Eigen::Vector3f *eye, Eigen::Vector3f *lookat, float roll) : eye_(*eye), lookat_(*lookat), roll_(roll) {}

Camera::~Camera() {}

void Camera::calculate_window(int width, int height, float perspective) {
    // i is a vector between the camera and the center of the window
    Eigen::Vector3f i = lookat_ - eye_;
    i *= (1.0f / i.norm());

    Eigen::Vector3f k;
    k << 0.0f, 0.0f, 1.0f;

    Eigen::Vector3f j = i.cross(k);
    j *= (1.0f / j.norm());

    k = j.cross(i);
    k *= (1.0f / k.norm());

    // Rotate camera around the i axis
    float roll = roll_ * M_PI / 180.0f;
    float sina = std::sin(roll);
    float cosa = std::cos(roll);

    Eigen::Vector3f jp = cosa * j + sina * k;
    Eigen::Vector3f kp = -sina * j + cosa * k;

    j = jp;
    k = kp;

    // Calculate the central point of the window
    Eigen::Vector3f center = eye_ + perspective * i;

    // Find three corners of the window
    wo_ = center - 0.5f * j + 0.5f * k;

    Eigen::Vector3f h = wo_ + j;
    Eigen::Vector3f v = wo_ - k;

    // Find vectors spanning the window
    wh_ = 1.0f / static_cast<float>(width) * (h - wo_);
    wv_ = 1.0f / static_cast<float>(height) * (v - wo_);
}

Eigen::Vector3f Camera::calculate_origin(int windowx, int windowy) {
    return (wo_ + static_cast<float>(windowx) * wh_ + static_cast<float>(windowy) * wv_);
}

Eigen::Vector3f Camera::calculate_direction(Eigen::Vector3f *origin) {
    Eigen::Vector3f direction = (*origin) - eye_;
    return (direction * (1.0f / direction.norm()));
}

} //namespace mrtp
