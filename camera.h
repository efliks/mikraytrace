#ifndef _CAMERA_H
#define _CAMERA_H

#include <Eigen/Core>


namespace mrtp {

class Camera {
  public:
    Camera(Eigen::Vector3d *eye, Eigen::Vector3d *lookat, double roll);
    ~Camera();
    void calculate_window(int width, int height, double perspective);
    Eigen::Vector3d calculate_origin(int windowx, int windowy);
    Eigen::Vector3d calculate_direction(Eigen::Vector3d *origin);

  private:
    double roll_;
    Eigen::Vector3d eye_;
    Eigen::Vector3d lookat_;
    Eigen::Vector3d wo_;
    Eigen::Vector3d wh_;
    Eigen::Vector3d wv_;
};

} //namespace mrtp

#endif //_CAMERA_H
