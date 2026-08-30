#ifndef MATRIX3_H
#define MATRIX3_H

#include "vector3.h"

/*
 * Minimal, plain C++98 replacement for the small slice of Eigen (Geometry)
 * actually used in this project: Matrix3d and elementary axis rotations.
 * It is not a general-purpose linear algebra library - only the operations
 * exercised elsewhere in the code base are provided.
 *
 * See vector3.h for the corresponding Vector3d / Vector3f replacement.
 */

namespace mrtp {

class Matrix3d {
public:
    Matrix3d();

    double& operator()(int row, int col);
    double operator()(int row, int col) const;

    Vector3d col(int index) const;

    Matrix3d operator*(const Matrix3d& other) const;
    Vector3d operator*(const Vector3d& vec) const;

private:
    double m_[3][3];
};

// Elementary rotations of `angle` radians around the X, Y, and Z axes.
Matrix3d rotation_x(double angle);
Matrix3d rotation_y(double angle);
Matrix3d rotation_z(double angle);

} // namespace mrtp

#endif // MATRIX3_H
