#include <Eigen/Geometry>
#include <cmath>
#include "actors.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;

const double kMyZero = 0.0001;


double solve_quadratic(double a, double b, double c) {
    double delta = b * b - 4 * a * c;
    if (delta < 0) {
        return -1;
    }

    if (-kMyZero < delta < kMyZero) {
        return -b / (2 * a);
    }

    double sqdelta = sqrt(delta);
    double t = 0.5 / a;
    double ta = (-b - sqdelta) * t;
    double tb = (-b + sqdelta) * t;

    return (ta < tb) ? ta : tb;
}


StandardBasis::StandardBasis(const Vector3d& o,
                             const Vector3d& i,
                             const Vector3d& j,
                             const Vector3d& k) :
    o(o), vi(i), vj(j), vk(k) {

}


StandardBasis::StandardBasis() {
    o << 0, 0, 0;
    vi << 1, 0, 0;
    vj << 0, 1, 0;
    vk << 0, 0, 1;
}


ActorBase::ActorBase(const StandardBasis& local_basis) :
    local_basis_(local_basis) {

}


TexturedPlane::TexturedPlane(const StandardBasis& local_basis,
                             MyTexture* texture) :
    ActorBase(local_basis), texture_(texture) {

}


bool TexturedPlane::has_shadow() const {
    return false;
}


double TexturedPlane::solve_light_ray(const Vector3d& O,
                                      const Vector3d& D,
                                      double min_dist,
                                      double max_dist) const {
    double t = D.dot(local_basis_.vk);

    if (t > kMyZero || t < -kMyZero) {
        Vector3d v = O - local_basis_.o;
        double d = -v.dot(local_basis_.vk) / t;
        if (d > min_dist && d < max_dist) {
            return d;
        }
    }
    return -1;
}


MyPixel TexturedPlane::pick_pixel(const Vector3d& hit,
                                  const Vector3d& normal_at_hit) const {
   Vector3d v = hit - local_basis_.o;

   double tx_i = v.dot(local_basis_.vi);
   double tx_j = v.dot(local_basis_.vj);

   return texture_->pick_pixel(tx_i, tx_j);
}


Vector3d TexturedPlane::calculate_normal_at_hit(const Vector3d& hit) const {
    return local_basis_.vk;
}


TexturedSphere::TexturedSphere(const StandardBasis& local_basis,
                               double radius,
                               MyTexture* texture) :
    ActorBase(local_basis), texture_(texture), radius_(radius) {

}


bool TexturedSphere::has_shadow() const {
    return true;
}


double TexturedSphere::solve_light_ray(const Vector3d& O,
                                       const Vector3d& D,
                                       double min_dist,
                                       double max_dist) const {
    Vector3d t = O - local_basis_.o;

    double a = D.dot(D);
    double b = 2 * D.dot(t);
    double c = t.dot(t) - radius_ * radius_;
    double dist = solve_quadratic(a, b, c);

    if (dist > min_dist && dist < max_dist) {
        return dist;
    }
    return -1;
}


MyPixel TexturedSphere::pick_pixel(const Vector3d& hit,
                                   const Vector3d& normal_at_hit) const {
    // Taken from https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html

    double dot = normal_at_hit.dot(local_basis_.vj);
    double phi = std::acos(-dot);
    double fracy = phi / M_PI;

    dot = normal_at_hit.dot(local_basis_.vi);
    double theta = std::acos(dot / std::sin(phi)) / (2 * M_PI);
    dot = normal_at_hit.dot(local_basis_.vk);
    double fracx = (dot > 0) ? theta : (1 - theta);

    return texture_->pick_pixel(fracx, fracy);
}


Vector3d TexturedSphere::calculate_normal_at_hit(const Vector3d& hit) const {
    Vector3d t = hit - local_basis_.o;
    return t * (1 / t.norm());
}


TexturedCylinder::TexturedCylinder(const StandardBasis& local_basis,
                                   double cylinder_radius,
                                   double cylinder_lenght,
                                   MyTexture* texture) :
    ActorBase(local_basis),
    texture_(texture),
    radius_(cylinder_radius),
    length_(cylinder_lenght) {

}


bool TexturedCylinder::has_shadow() const {
    return true;
}


/*
Capital letters are vectors.
  A       Origin    of cylinder
  B       Direction of cylinder
  O       Origin    of ray
  D       Direction of ray
  P       Hit point on cylinder's surface
  X       Point on cylinder's axis closest to the hit point
  t       Distance between ray's      origin and P
  alpha   Distance between cylinder's origin and X

 (P - X) . B = 0
 |P - X| = R  => (P - X) . (P - X) = R^2

 P = O + t * D
 X = A + alpha * B
 T = O - A
 ...
 2t * (T.D - alpha * D.B)  +  t^2 - 2 * alpha * T.B  +
     +  alpha^2  =  R^2 - T.T
 a = T.D
 b = D.B
 d = T.B
 f = R^2 - T.T

 t^2 * (1 - b^2)  +  2t * (a - b * d)  -
     -  d^2 - f = 0    => t = ...
 alpha = d + t * b
*/
double TexturedCylinder::solve_light_ray(const Vector3d& O,
                                         const Vector3d& D,
                                         double min_dist,
                                         double max_dist) const {
    Vector3d vec = O - local_basis_.o;

    double a = D.dot(vec);
    double b = D.dot(local_basis_.vk);
    double d = vec.dot(local_basis_.vk);
    double f = radius_ * radius_ - vec.dot(vec);

    // Solving quadratic equation for t
    double aa = 1 - (b * b);
    double bb = 2 * (a - b * d);
    double cc = -(d * d) - f;
    double t = solve_quadratic(aa, bb, cc);

    if (t < min_dist || t > max_dist) {
        return -1;
    }
    // Check if cylinder is finite
    if (length_ > 0) {
        double alpha = d + t * b;
        if (alpha < -length_ || alpha > length_) {
            return -1;
        }
    }
    return t;
}


Vector3d TexturedCylinder::calculate_normal_at_hit(const Vector3d& hit) const {
    // N = Hit - [B . (Hit - A)] * B
    Vector3d v = hit - local_basis_.o;
    double alpha = local_basis_.vk.dot(v);
    Vector3d w = local_basis_.o + alpha * local_basis_.vk;
    Vector3d normal = hit - w;

    return normal * (1 / normal.norm());
}


MyPixel TexturedCylinder::pick_pixel(const Vector3d& hit,
                                     const Vector3d& normal_at_hit) const {
    Vector3d t = hit - local_basis_.o;

    double alpha = t.dot(local_basis_.vk);
    double dot = normal_at_hit.dot(local_basis_.vi);
    double frac_x = acos(dot) / M_PI;
    double frac_y = alpha / (2 * M_PI * radius_);

    return texture_->pick_pixel(frac_x, frac_y);
}


}  // namespace mrtp

