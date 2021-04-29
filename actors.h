#ifndef _ACTORS_H
#define _ACTORS_H

#include <Eigen/Core>
#include "pixel.h"
#include "texture.h"


namespace mrtp {

class Actor {
public:
    Actor() = default;
    virtual ~Actor() = default;

    virtual double solve(
        Eigen::Vector3d* origin,
        Eigen::Vector3d* direction,
        double mind, double maxd) = 0;

    virtual Pixel pick_pixel(
        Eigen::Vector3d* hit,
        Eigen::Vector3d* normal) = 0;

    virtual Eigen::Vector3d calculate_normal(
        Eigen::Vector3d* hit) = 0;

    bool has_shadow;
    double reflect_coeff;

protected:
    Texture* texture_;
};


class Plane : public Actor {
public:
    Plane(
        Eigen::Vector3d* center,
        Eigen::Vector3d* normal,
        double scale, double reflect,
        const char* texture);

    virtual ~Plane() override = default;

    virtual double solve(
        Eigen::Vector3d* origin,
        Eigen::Vector3d* direction,
        double mind, double maxd) override;

    virtual Pixel pick_pixel(
        Eigen::Vector3d* hit,
        Eigen::Vector3d* normal) override;

    virtual Eigen::Vector3d calculate_normal(
        Eigen::Vector3d* hit) override;

private:
    Eigen::Vector3d center_;
    Eigen::Vector3d normal_;
    Eigen::Vector3d tx_;
    Eigen::Vector3d ty_;

    double scale_;
};


class Sphere : public Actor {
public:
    Sphere(
        Eigen::Vector3d* center,
        Eigen::Vector3d* axis,
        double radius,
        double reflect,
        const char* texture);

    virtual ~Sphere() override = default;

    virtual double solve(
        Eigen::Vector3d* origin,
        Eigen::Vector3d* direction,
        double mind, double maxd) override;

    virtual Pixel pick_pixel(
        Eigen::Vector3d* hit,
        Eigen::Vector3d* normal) override;

    virtual Eigen::Vector3d calculate_normal(
        Eigen::Vector3d* hit) override;

private:
    Eigen::Vector3d center_;
    Eigen::Vector3d tx_;
    Eigen::Vector3d ty_;
    Eigen::Vector3d tz_;

    double R_;
};


class Cylinder : public Actor {
public:
    Cylinder(
        Eigen::Vector3d* center,
        Eigen::Vector3d* direction,
        double radius, double span, double reflect,
        const char* texture);

    virtual ~Cylinder() override = default;

    virtual double solve(
        Eigen::Vector3d* origin,
        Eigen::Vector3d* direction,
        double mind, double maxd) override;

    virtual Pixel pick_pixel(
        Eigen::Vector3d* hit,
        Eigen::Vector3d* normal) override;

    virtual Eigen::Vector3d calculate_normal(
        Eigen::Vector3d* hit) override;

private:
    Eigen::Vector3d A_;
    Eigen::Vector3d B_;
    Eigen::Vector3d tx_;
    Eigen::Vector3d ty_;

    double R_;
    double span_;
};


}  // namespace mrtp

#endif //_ACTORS_H
