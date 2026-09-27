#ifndef TOOLS_H
#define TOOLS_H

#include "shrptr.h"
#include <string>
#include "matrix3.h"

#include "config.h"
#include "common.h"

namespace mrtp {

extern const double kMyZero;

double solve_quadratic(double, double, double);

Vector3d fill_vector(const Vector3d&);

Matrix3d create_rotation_matrix(shared_ptr<ConfigTable>, const std::string& = "");

void set_basis(StandardBasis*, const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&);

}

#endif
