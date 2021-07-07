#ifndef ACTORS_H
#define ACTORS_H

#include <memory>
#include <Eigen/Core>
#include "pixel.h"
#include "texture.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;


class StandardBasis {
public:
    StandardBasis(const Vector3d&, const Vector3d&,
                    const Vector3d&, const Vector3d&);
    StandardBasis();
    ~StandardBasis() = default;

    Vector3d o;
    Vector3d vi;
    Vector3d vj;
    Vector3d vk;
};


class ActorBase {
public:
    ActorBase(const StandardBasis&);
    ActorBase() = default;
    virtual ~ActorBase() = default;

    virtual double solve_light_ray(const Vector3d&, const Vector3d&, double,
                                   double) const = 0;
    virtual Pixel pick_pixel(const Vector3d&, const Vector3d&) const = 0;
    virtual Vector3d calculate_normal_at_hit(const Vector3d&) const = 0;
    virtual bool has_shadow() const = 0;

protected:
    StandardBasis local_basis_;
};


class TexturedPlane : public ActorBase {
public:
    TexturedPlane(const StandardBasis&, Texture*);  // texture has scale and reflection
    ~TexturedPlane() override = default;

    double solve_light_ray(const Vector3d&, const Vector3d&, double,
                           double) const override;
    Pixel pick_pixel(const Vector3d&, const Vector3d&) const override;
    Vector3d calculate_normal_at_hit(const Vector3d&) const override;
    bool has_shadow() const override;

private:
    Texture* texture_;
};


class TexturedSphere : public ActorBase {
public:
    TexturedSphere(const StandardBasis&, double, Texture*);
    ~TexturedSphere() override = default;

    double solve_light_ray(const Vector3d&, const Vector3d&, double,
                           double) const override;
    Pixel pick_pixel(const Vector3d&, const Vector3d&) const override;
    Vector3d calculate_normal_at_hit(const Vector3d&) const override;
    bool has_shadow() const override;

private:
    Texture* texture_;

    double radius_;
};


class TexturedCylinder : public ActorBase {
public:
    TexturedCylinder(const StandardBasis&, double, double, Texture*);
    ~TexturedCylinder() override = default;

    double solve_light_ray(const Vector3d&, const Vector3d&, double,
                           double) const override;
    Pixel pick_pixel(const Vector3d&, const Vector3d&) const override;
    Vector3d calculate_normal_at_hit(const Vector3d&) const override;
    bool has_shadow() const override;

private:
    Texture* texture_;

    double radius_, length_;
};


}

#endif // ACTORS_H
