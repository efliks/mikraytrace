#include <cmath>

#include "matrix3.h"

namespace mrtp {

// ---------------------------------------------------------------- Matrix3

Matrix3::Matrix3()
{
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            m_[row][col] = 0;
        }
    }
}

double& Matrix3::operator()(int row, int col)
{
    return m_[row][col];
}

double Matrix3::operator()(int row, int col) const
{
    return m_[row][col];
}

Vector3 Matrix3::col(int index) const
{
    return Vector3(m_[0][index], m_[1][index], m_[2][index]);
}

Matrix3 Matrix3::operator*(const Matrix3& other) const
{
    Matrix3 result;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            double sum = 0;
            for (int k = 0; k < 3; k++) {
                sum += m_[row][k] * other.m_[k][col];
            }
            result.m_[row][col] = sum;
        }
    }
    return result;
}

Vector3 Matrix3::operator*(const Vector3& vec) const
{
    return Vector3(
        m_[0][0] * vec[0] + m_[0][1] * vec[1] + m_[0][2] * vec[2],
        m_[1][0] * vec[0] + m_[1][1] * vec[1] + m_[1][2] * vec[2],
        m_[2][0] * vec[0] + m_[2][1] * vec[1] + m_[2][2] * vec[2]);
}

// ------------------------------------------------------ Elementary rotations

Matrix3 rotation_x(double angle)
{
    double c = std::cos(angle);
    double s = std::sin(angle);

    Matrix3 result;
    result(0, 0) = 1;
    result(1, 1) = c;
    result(1, 2) = -s;
    result(2, 1) = s;
    result(2, 2) = c;
    return result;
}

Matrix3 rotation_y(double angle)
{
    double c = std::cos(angle);
    double s = std::sin(angle);

    Matrix3 result;
    result(0, 0) = c;
    result(0, 2) = s;
    result(1, 1) = 1;
    result(2, 0) = -s;
    result(2, 2) = c;
    return result;
}

Matrix3 rotation_z(double angle)
{
    double c = std::cos(angle);
    double s = std::sin(angle);

    Matrix3 result;
    result(0, 0) = c;
    result(0, 1) = -s;
    result(1, 0) = s;
    result(1, 1) = c;
    result(2, 2) = 1;
    return result;
}

} // namespace mrtp
