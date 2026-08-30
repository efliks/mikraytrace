#ifndef VECTOR3_H
#define VECTOR3_H

/*
 * Minimal, plain C++98 replacement for the small slice of Eigen (Core)
 * actually used in this project: Vector3d and Vector3f. It is not a
 * general-purpose linear algebra library - only the operations exercised
 * elsewhere in the code base are provided.
 *
 * See matrix3.h for the corresponding Matrix3d / AngleAxisd replacement.
 */

namespace mrtp {

class Vector3d {
public:
    Vector3d();
    Vector3d(double x, double y, double z);
    explicit Vector3d(const double* data);

    double& operator[](int index);
    double operator[](int index) const;

    int size() const;

    Vector3d operator-() const;

    Vector3d& operator+=(const Vector3d& other);
    Vector3d& operator-=(const Vector3d& other);
    Vector3d& operator*=(double scalar);
    Vector3d& operator/=(double scalar);

    double dot(const Vector3d& other) const;
    Vector3d cross(const Vector3d& other) const;
    double norm() const;

    static Vector3d UnitX();
    static Vector3d UnitY();
    static Vector3d UnitZ();

private:
    double v_[3];
};

Vector3d operator+(const Vector3d& lhs, const Vector3d& rhs);
Vector3d operator-(const Vector3d& lhs, const Vector3d& rhs);
Vector3d operator*(const Vector3d& vec, double scalar);
Vector3d operator*(double scalar, const Vector3d& vec);
Vector3d operator/(const Vector3d& vec, double scalar);

// Single-precision, 12-byte-packed vector used only for reading raw
// vertex data from binary mesh files, and converting it to a Vector3d.
class Vector3f {
public:
    Vector3f();
    Vector3f(float x, float y, float z);

    Vector3d to_vector3() const;

private:
    float v_[3];
};

} // namespace mrtp

#endif // VECTOR3_H
