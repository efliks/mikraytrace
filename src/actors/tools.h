#ifndef TOOLS_H
#define TOOLS_H

#include <memory>
#include <Eigen/Core>

#include "cpptoml.h"

using Vector3d = Eigen::Vector3d;


namespace mrtp {

extern const double kMyZero;

double solve_quadratic(double, double, double);

Vector3d fill_vector(const Vector3d&);

Eigen::Matrix3d create_rotation_matrix(std::shared_ptr<cpptoml::table>);

}

#endif
