#include <cmath>

#include "vector3.h"

namespace mrtp {

// ---------------------------------------------------------------- Vector3d

Vector3d::Vector3d()
{
    v_[0] = 0;
    v_[1] = 0;
    v_[2] = 0;
}

Vector3d::Vector3d(double x, double y, double z)
{
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
}

Vector3d::Vector3d(const double* data)
{
    v_[0] = data[0];
    v_[1] = data[1];
    v_[2] = data[2];
}

double& Vector3d::operator[](int index)
{
    return v_[index];
}

double Vector3d::operator[](int index) const
{
    return v_[index];
}

int Vector3d::size() const
{
    return 3;
}

Vector3d Vector3d::operator-() const
{
    return Vector3d(-v_[0], -v_[1], -v_[2]);
}

Vector3d& Vector3d::operator+=(const Vector3d& other)
{
    v_[0] += other.v_[0];
    v_[1] += other.v_[1];
    v_[2] += other.v_[2];
    return *this;
}

Vector3d& Vector3d::operator-=(const Vector3d& other)
{
    v_[0] -= other.v_[0];
    v_[1] -= other.v_[1];
    v_[2] -= other.v_[2];
    return *this;
}

Vector3d& Vector3d::operator*=(double scalar)
{
    v_[0] *= scalar;
    v_[1] *= scalar;
    v_[2] *= scalar;
    return *this;
}

Vector3d& Vector3d::operator/=(double scalar)
{
    v_[0] /= scalar;
    v_[1] /= scalar;
    v_[2] /= scalar;
    return *this;
}

double Vector3d::dot(const Vector3d& other) const
{
    return v_[0] * other.v_[0] + v_[1] * other.v_[1] + v_[2] * other.v_[2];
}

Vector3d Vector3d::cross(const Vector3d& other) const
{
    return Vector3d(
        v_[1] * other.v_[2] - v_[2] * other.v_[1],
        v_[2] * other.v_[0] - v_[0] * other.v_[2],
        v_[0] * other.v_[1] - v_[1] * other.v_[0]);
}

double Vector3d::norm() const
{
    return std::sqrt(dot(*this));
}

Vector3d Vector3d::UnitX()
{
    return Vector3d(1, 0, 0);
}

Vector3d Vector3d::UnitY()
{
    return Vector3d(0, 1, 0);
}

Vector3d Vector3d::UnitZ()
{
    return Vector3d(0, 0, 1);
}

Vector3d operator+(const Vector3d& lhs, const Vector3d& rhs)
{
    Vector3d result(lhs);
    result += rhs;
    return result;
}

Vector3d operator-(const Vector3d& lhs, const Vector3d& rhs)
{
    Vector3d result(lhs);
    result -= rhs;
    return result;
}

Vector3d operator*(const Vector3d& vec, double scalar)
{
    Vector3d result(vec);
    result *= scalar;
    return result;
}

Vector3d operator*(double scalar, const Vector3d& vec)
{
    return vec * scalar;
}

Vector3d operator/(const Vector3d& vec, double scalar)
{
    Vector3d result(vec);
    result /= scalar;
    return result;
}

// --------------------------------------------------------------- Vector3f

Vector3f::Vector3f()
{
    v_[0] = 0;
    v_[1] = 0;
    v_[2] = 0;
}

Vector3f::Vector3f(float x, float y, float z)
{
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
}

Vector3d Vector3f::to_vector3() const
{
    return Vector3d(
        static_cast<double>(v_[0]),
        static_cast<double>(v_[1]),
        static_cast<double>(v_[2]));
}

} // namespace mrtp
