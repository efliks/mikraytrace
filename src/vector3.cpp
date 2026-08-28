#include <cmath>

#include "vector3.h"

namespace mrtp {

// ---------------------------------------------------------------- Vector3

Vector3::Vector3()
{
    v_[0] = 0;
    v_[1] = 0;
    v_[2] = 0;
}

Vector3::Vector3(double x, double y, double z)
{
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
}

Vector3::Vector3(const double* data)
{
    v_[0] = data[0];
    v_[1] = data[1];
    v_[2] = data[2];
}

double& Vector3::operator[](int index)
{
    return v_[index];
}

double Vector3::operator[](int index) const
{
    return v_[index];
}

int Vector3::size() const
{
    return 3;
}

Vector3 Vector3::operator-() const
{
    return Vector3(-v_[0], -v_[1], -v_[2]);
}

Vector3& Vector3::operator+=(const Vector3& other)
{
    v_[0] += other.v_[0];
    v_[1] += other.v_[1];
    v_[2] += other.v_[2];
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other)
{
    v_[0] -= other.v_[0];
    v_[1] -= other.v_[1];
    v_[2] -= other.v_[2];
    return *this;
}

Vector3& Vector3::operator*=(double scalar)
{
    v_[0] *= scalar;
    v_[1] *= scalar;
    v_[2] *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(double scalar)
{
    v_[0] /= scalar;
    v_[1] /= scalar;
    v_[2] /= scalar;
    return *this;
}

double Vector3::dot(const Vector3& other) const
{
    return v_[0] * other.v_[0] + v_[1] * other.v_[1] + v_[2] * other.v_[2];
}

Vector3 Vector3::cross(const Vector3& other) const
{
    return Vector3(
        v_[1] * other.v_[2] - v_[2] * other.v_[1],
        v_[2] * other.v_[0] - v_[0] * other.v_[2],
        v_[0] * other.v_[1] - v_[1] * other.v_[0]);
}

double Vector3::norm() const
{
    return std::sqrt(dot(*this));
}

Vector3 Vector3::UnitX()
{
    return Vector3(1, 0, 0);
}

Vector3 Vector3::UnitY()
{
    return Vector3(0, 1, 0);
}

Vector3 Vector3::UnitZ()
{
    return Vector3(0, 0, 1);
}

Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
{
    Vector3 result(lhs);
    result += rhs;
    return result;
}

Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
{
    Vector3 result(lhs);
    result -= rhs;
    return result;
}

Vector3 operator*(const Vector3& vec, double scalar)
{
    Vector3 result(vec);
    result *= scalar;
    return result;
}

Vector3 operator*(double scalar, const Vector3& vec)
{
    return vec * scalar;
}

Vector3 operator/(const Vector3& vec, double scalar)
{
    Vector3 result(vec);
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

Vector3 Vector3f::to_vector3() const
{
    return Vector3(
        static_cast<double>(v_[0]),
        static_cast<double>(v_[1]),
        static_cast<double>(v_[2]));
}

} // namespace mrtp
