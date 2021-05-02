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
        const Eigen::Vector3d& origin,
        const Eigen::Vector3d& direction,
        double mind, double maxd) const = 0;

    virtual Pixel pick_pixel(
        const Eigen::Vector3d& hit,
        const Eigen::Vector3d& normal) const = 0;

    virtual Eigen::Vector3d calculate_normal(
        const Eigen::Vector3d& hit) const = 0;

    bool has_shadow;
    double reflect_coeff;

protected:
    Texture* texture_;
};


class Plane : public Actor {
public:
    Plane(const Eigen::Vector3d& center, const Eigen::Vector3d& normal,
          double scale, double reflect, const std::string& texture_filename,
          TextureCollector& texture_collector);

    virtual ~Plane() override = default;

    virtual double solve(
        const Eigen::Vector3d& origin,
        const Eigen::Vector3d& direction,
        double mind, double maxd) const override;

    virtual Pixel pick_pixel(
        const Eigen::Vector3d& hit,
        const Eigen::Vector3d& normal) const override;

    virtual Eigen::Vector3d calculate_normal(
        const Eigen::Vector3d& hit) const override;

private:
    Eigen::Vector3d center_;
    Eigen::Vector3d normal_;
    Eigen::Vector3d tx_;
    Eigen::Vector3d ty_;

    double scale_;
};


class Sphere : public Actor {
public:
    Sphere(const Eigen::Vector3d& center, const Eigen::Vector3d& axis,
           double radius, double reflect, const std::string& texture_filename,
           TextureCollector& texture_collector);

    virtual ~Sphere() override = default;

    virtual double solve(
        const Eigen::Vector3d& origin,
        const Eigen::Vector3d& direction,
        double mind, double maxd) const override;

    virtual Pixel pick_pixel(
        const Eigen::Vector3d& hit,
        const Eigen::Vector3d& normal) const override;

    virtual Eigen::Vector3d calculate_normal(
        const Eigen::Vector3d& hit) const override;

private:
    Eigen::Vector3d center_;
    Eigen::Vector3d tx_;
    Eigen::Vector3d ty_;
    Eigen::Vector3d tz_;

    double R_;
};


class Cylinder : public Actor {
public:
    Cylinder(const Eigen::Vector3d& center, const Eigen::Vector3d& direction,
             double radius, double span, double reflect,
             const std::string& texture_filename,
             TextureCollector& texture_collector);

    virtual ~Cylinder() override = default;

    virtual double solve(
        const Eigen::Vector3d& origin,
        const Eigen::Vector3d& direction,
        double mind, double maxd) const override;

    virtual Pixel pick_pixel(
        const Eigen::Vector3d& hit,
        const Eigen::Vector3d& normal) const override;

    virtual Eigen::Vector3d calculate_normal(
        const Eigen::Vector3d& hit) const override;

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
