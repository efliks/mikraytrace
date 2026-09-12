#include <cmath>
#include "matrix3.h"

#include "actors/tools.h"
#include "common.h"

namespace {
double pi() { return std::atan(1.0) * 4; }
}


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

    double sqdelta = std::sqrt(delta);
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

    Vector3d unit(0, 0, 1);

    if (x < y) {
        if (x < z) {
            unit = Vector3d(1, 0, 0);
        }
    } else { // if ( x >= y)
        if (y < z) {
            unit = Vector3d(0, 1, 0);
        }
    }
    return unit;
}


Matrix3d create_rotation_matrix(shared_ptr<ConfigTable> items,
                                const std::string& prefix)
{
    double angle_x = items->get_value(prefix + "angle_x", 0) * pi() / 180;
    double angle_y = items->get_value(prefix + "angle_y", 0) * pi() / 180;
    double angle_z = items->get_value(prefix + "angle_z", 0) * pi() / 180;

    return rotation_x(angle_x) * rotation_y(angle_y) * rotation_z(angle_z);
}


// Helper function for DJGPP
void set_basis(StandardBasis* sb, const Vector3d& o, const Vector3d& i,
        const Vector3d& j, const Vector3d& k)
{
    sb->o = o;
    sb->vi = i;
    sb->vj = j;
    sb->vk = k;
}


}
