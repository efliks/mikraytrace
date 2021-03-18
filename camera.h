/* File      : camera.h
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#ifndef _CAMERA_H
#define _CAMERA_H

#include <Eigen/Core>


namespace mrtp {

class Camera {
  public:
    Camera(Eigen::Vector3f *eye, Eigen::Vector3f *lookat, float roll);
    ~Camera();
    void calculate_window(int width, int height, float perspective);
    Eigen::Vector3f calculate_origin(int windowx, int windowy);
    Eigen::Vector3f calculate_direction(Eigen::Vector3f *origin);

  private:
    float roll_;
    Eigen::Vector3f eye_;
    Eigen::Vector3f lookat_;
    Eigen::Vector3f wo_;
    Eigen::Vector3f wh_;
    Eigen::Vector3f wv_;
};

} //namespace mrtp

#endif //_CAMERA_H
