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

class Matrix3 {
public:
    Matrix3();

    double& operator()(int row, int col);
    double operator()(int row, int col) const;

    Vector3 col(int index) const;

    Matrix3 operator*(const Matrix3& other) const;
    Vector3 operator*(const Vector3& vec) const;

private:
    double m_[3][3];
};

// Elementary rotations of `angle` radians around the X, Y, and Z axes.
Matrix3 rotation_x(double angle);
Matrix3 rotation_y(double angle);
Matrix3 rotation_z(double angle);

} // namespace mrtp

#endif // MATRIX3_H
