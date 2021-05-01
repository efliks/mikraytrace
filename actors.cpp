#include <Eigen/Geometry>
#include <cmath>
#include "actors.h"


namespace mrtp {

const double kAlmostZero = 0.0001;


double solve_quadratic(double a, double b, double c,
                       double mint, double maxt) {
    double t = -1;
    double delta = b * b - 4 * a * c;

    if (delta >= 0) {
        if (delta > 0) {
            double sqdelta = sqrt(delta);
            double tmp = 0.5 / a;
            double ta = (-b - sqdelta) * tmp;
            double tb = (-b + sqdelta) * tmp;
            t = (ta < tb) ? ta : tb;
        } else {
            t = -b / (2 * a);
        }
        if ((t < mint) || (t > maxt)) {
            t = -1;
        }
    }
    return t;
}


Eigen::Vector3d generate_unit_vector(const Eigen::Vector3d& vector) {
    double tx = vector[0];
    double ty = vector[1];
    double tz = vector[2];

    double x = (tx < 0) ? -tx : tx;
    double y = (ty < 0) ? -ty : ty;
    double z = (tz < 0) ? -tz : tz;

    Eigen::Vector3d unit;
    unit << 0, 0, 1;

    if (x < y) {
        if (x < z) {
            unit << 1, 0, 0;
        }
    } else { // if ( x >= y)
        if (y < z) {
            unit << 0, 1, 0;
        }
    }
    return unit;
}


Plane::Plane(const Eigen::Vector3d& center,
             const Eigen::Vector3d& normal,
             double scale, double reflect,
             const char* texture) {
    center_ = center;
    normal_ = (1 / normal.norm()) * normal;
    scale_ = scale;
    reflect_coeff = reflect;
    has_shadow = false;

    Eigen::Vector3d tmp = generate_unit_vector(normal_);
    tx_ = tmp.cross(normal_);
    tx_ *= (1 / tx_.norm());
    ty_ = normal_.cross(tx_);
    ty_ *= (1 / ty_.norm());

    texture_ = textureCollector.add(texture);
}


Pixel Plane::pick_pixel(const Eigen::Vector3d& hit,
                        const Eigen::Vector3d& normal) const {
    Eigen::Vector3d v = hit - center_;
    // Calculate components of v (dot products)
    double vx = v.dot(tx_);
    double vy = v.dot(ty_);

    return texture_->pick_pixel(vx, vy, scale_);
}


double Plane::solve(const Eigen::Vector3d& origin,
                    const Eigen::Vector3d& direction,
                    double mind, double maxd) const {
  double bar = direction.dot(normal_);
    if (bar > kAlmostZero or bar < -kAlmostZero) {
        Eigen::Vector3d tmp = origin - center_;
        double d = -tmp.dot(normal_) / bar;
        if ((d >= mind) && (d <= maxd)) {
            return d;
        }
    }
    return -1;
}


Eigen::Vector3d Plane::calculate_normal(const Eigen::Vector3d& hit) const {
    return normal_;
}


Sphere::Sphere(const Eigen::Vector3d& center,
               const Eigen::Vector3d& axis,
               double radius, double reflect,
               const char* texture) {
    center_ = center;
    R_ = radius;
    has_shadow = true;
    reflect_coeff = reflect;

    ty_ = axis;
    ty_ *= (1 / ty_.norm());

    Eigen::Vector3d tmp = generate_unit_vector(ty_);
    tx_ = tmp.cross(ty_);
    tx_ *= (1 / tx_.norm());
    tz_ = ty_.cross(tx_);
    tz_ *= (1 / tz_.norm());

    texture_ = textureCollector.add(texture);
}


double Sphere::solve(const Eigen::Vector3d& origin,
                     const Eigen::Vector3d& direction,
                     double mind, double maxd) const {
    Eigen::Vector3d t = origin - center_;
    double a = direction.dot(direction);
    double b = 2 * direction.dot(t);
    double c = t.dot(t) - (R_ * R_);

    return solve_quadratic(a, b, c, mind, maxd);
}


Eigen::Vector3d Sphere::calculate_normal(const Eigen::Vector3d& hit) const {
    Eigen::Vector3d normal = hit - center_;
    return (normal * (1 / normal.norm()));
}


/*
Guidelines:
https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html
*/
Pixel Sphere::pick_pixel(const Eigen::Vector3d& hit,
                         const Eigen::Vector3d& normal) const {
    double dot = normal.dot(ty_);
    double phi = std::acos(-dot);
    double fracy = phi / M_PI;

    dot = normal.dot(tx_);
    double theta = std::acos(dot / std::sin(phi)) / (2 * M_PI);
    dot = normal.dot(tz_);
    double fracx = (dot > 0) ? theta : (1 - theta);

    return texture_->pick_pixel(fracx, fracy, 1);
}


Cylinder::Cylinder(const Eigen::Vector3d& center,
                   const Eigen::Vector3d& direction,
                   double radius, double span, double reflect,
                   const char* texture) {
    A_ = center;
    B_ = direction;
    B_ *= (1 / B_.norm());
    R_ = radius;

    span_ = span;
    reflect_coeff = reflect;
    has_shadow = true;

    ty_ = generate_unit_vector(B_);
    tx_ = ty_.cross(B_);
    tx_ *= (1 / tx_.norm());

    texture_ = textureCollector.add(texture);
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
double Cylinder::solve(const Eigen::Vector3d& O,
                       const Eigen::Vector3d& D,
                      double mind, double maxd) const {
    Eigen::Vector3d tmp = O - A_;

    double a = D.dot(tmp);
    double b = D.dot(B_);
    double d = tmp.dot(B_);
    double f = (R_ * R_) - tmp.dot(tmp);

    // Solving a quadratic equation for t
    double aa = 1 - (b * b);
    double bb = 2 * (a - b * d);
    double cc = -(d * d) - f;
    double t = solve_quadratic(aa, bb, cc, mind, maxd);

    if (t > 0) {
        // Check if the cylinder is finite
        if (span_ > 0) {
            double alpha = d + t * b;
            if ((alpha < -span_) || (alpha > span_)) {
                return -1;
            }
        }
    }
    return t;
}


Eigen::Vector3d Cylinder::calculate_normal(const Eigen::Vector3d& hit) const {
    // N = Hit - [B . (Hit - A)] * B
    Eigen::Vector3d tmp = hit - A_;
    double alpha = B_.dot(tmp);
    Eigen::Vector3d bar = A_ + alpha * B_;
    Eigen::Vector3d normal = hit - bar;

    return (normal * (1 / normal.norm()));
}


Pixel Cylinder::pick_pixel(const Eigen::Vector3d& hit,
                           const Eigen::Vector3d& normal) const {
    Eigen::Vector3d tmp = hit - A_;
    double alpha = tmp.dot(B_);
    double dot = normal.dot(tx_);
    double fracx = acos(dot) / M_PI;
    double fracy = alpha / (2 * M_PI * R_);

    return texture_->pick_pixel(fracx, fracy, 1);
}

}  // namespace mrtp

