#include <cmath>
#include <Eigen/Geometry>

#include "actors/tools.h"


namespace mrtp 
{

const double kMyZero = 0.0001;


double solve_quadratic(double a, double b, double c) 
{
    double delta = b * b - 4 * a * c;
    if (delta < 0) {
        return -1;
    }

    if (delta < kMyZero && -delta > -kMyZero) {
        return -b / (2 * a);
    }

    double sqdelta = sqrt(delta);
    double t = 0.5 / a;
    double ta = (-b - sqdelta) * t;
    double tb = (-b + sqdelta) * t;

    return (ta < tb) ? ta : tb;
}


Vector3d fill_vector(const Vector3d& vec) 
{
    double x = (vec[0] < 0) ? -vec[0] : vec[0];
    double y = (vec[1] < 0) ? -vec[1] : vec[1];
    double z = (vec[2] < 0) ? -vec[2] : vec[2];

    Vector3d unit{0, 0, 1};

    if (x < y) {
        if (x < z) {
            unit << 1, 0, 0;
        }
    } else { // if ( x >= y)
        if (y < z) {
            unit << 0, 1, 0;
        }
    }
    return unit;
}


Eigen::Matrix3d create_rotation_matrix(std::shared_ptr<ConfigTable> items) 
{
    double angle_x = items->get_value("angle_x", 0);
    Eigen::AngleAxisd m_x = Eigen::AngleAxisd(angle_x * M_PI / 180, Vector3d::UnitX());

    double angle_y = items->get_value("angle_y", 0);
    Eigen::AngleAxisd m_y = Eigen::AngleAxisd(angle_y * M_PI / 180, Vector3d::UnitY());

    double angle_z = items->get_value("angle_z", 0);
    Eigen::AngleAxisd m_z = Eigen::AngleAxisd(angle_z * M_PI / 180, Vector3d::UnitZ());

    Eigen::Matrix3d m_rot;
    m_rot = m_x * m_y * m_z;

    return m_rot;
}


}
