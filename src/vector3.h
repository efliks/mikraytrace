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

class Vector3 {
public:
    Vector3();
    Vector3(double x, double y, double z);
    explicit Vector3(const double* data);

    double& operator[](int index);
    double operator[](int index) const;

    int size() const;

    Vector3 operator-() const;

    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(double scalar);
    Vector3& operator/=(double scalar);

    double dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    double norm() const;

    static Vector3 UnitX();
    static Vector3 UnitY();
    static Vector3 UnitZ();

private:
    double v_[3];
};

Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
Vector3 operator*(const Vector3& vec, double scalar);
Vector3 operator*(double scalar, const Vector3& vec);
Vector3 operator/(const Vector3& vec, double scalar);

// Single-precision, 12-byte-packed vector used only for reading raw
// vertex data from binary mesh files, and converting it to a Vector3.
class Vector3f {
public:
    Vector3f();
    Vector3f(float x, float y, float z);

    Vector3 to_vector3() const;

private:
    float v_[3];
};

} // namespace mrtp

#endif // VECTOR3_H
