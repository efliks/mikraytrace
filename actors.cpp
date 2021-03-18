/* File      : actors.cpp
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#include <Eigen/Geometry>
#include <cmath>

#include "actors.h"


namespace mrtp {

float solve_quadratic(float a, float b, float c, float mint,
                      float maxt) {
    float t = -1.0f;
    float delta = b * b - 4.0f * a * c;

    if (delta >= 0.0f) {
        if (delta > 0.0f) {
            float sqdelta = sqrt(delta);
            float tmp = 0.5f / a;
            float ta = (-b - sqdelta) * tmp;
            float tb = (-b + sqdelta) * tmp;
            t = (ta < tb) ? ta : tb;
        } else {
            t = -b / (2.0f * a);
        }

        if ((t < mint) || (t > maxt)) {
            t = -1.0f;
        }
    }
    return t;
}


Eigen::Vector3f generate_unit_vector(Eigen::Vector3f *vector) {
    float tx = (*vector)[0];
    float ty = (*vector)[1];
    float tz = (*vector)[2];

    float x = (tx < 0.0f) ? -tx : tx;
    float y = (ty < 0.0f) ? -ty : ty;
    float z = (tz < 0.0f) ? -tz : tz;

    Eigen::Vector3f unit;
    unit << 0.0f, 0.0f, 1.0f;

    if (x < y) {
        if (x < z) {
            unit << 1.0f, 0.0f, 0.0f;
        }
    } else { // if ( x >= y)
        if (y < z) {
            unit << 0.0f, 1.0f, 0.0f;
        }
    }
    return unit;
}


Actor::Actor() {
}


Actor::~Actor() {
}


bool Actor::has_shadow() {
    return has_shadow_;
}


float Actor::get_reflect() {
    return reflect_;
}


Plane::Plane(Eigen::Vector3f *center, Eigen::Vector3f *normal, float scale, 
             float reflect, const char *texture) {

    center_ = *center;
    normal_ = (1.0f / normal->norm()) * (*normal);
    scale_ = scale;
    reflect_ = reflect;
    has_shadow_ = false;

    Eigen::Vector3f tmp = generate_unit_vector(&normal_);
    tx_ = tmp.cross(normal_);
    tx_ *= (1.0f / tx_.norm());
    ty_ = normal_.cross(tx_);
    ty_ *= (1.0f / ty_.norm());

    texture_ = textureCollector.add(texture);
}


Plane::~Plane() {
}


Pixel Plane::pick_pixel(Eigen::Vector3f *hit, Eigen::Vector3f *normal) {
    Eigen::Vector3f v = (*hit) - center_;
    // Calculate components of v (dot products)
    float vx = v.dot(tx_);
    float vy = v.dot(ty_);

    return texture_->pick_pixel(vx, vy, scale_);
}


float Plane::solve(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                   float mind, float maxd) {
    float bar = direction->dot(normal_);

    if (bar != 0.0f) {
        Eigen::Vector3f tmp = (*origin) - center_;
        float d = -tmp.dot(normal_) / bar;
        if ((d >= mind) && (d <= maxd)) {
            return d;
        }
    }
    return -1.0f;
}


Eigen::Vector3f Plane::calculate_normal(Eigen::Vector3f *hit) {
    return normal_;
}


Sphere::Sphere(Eigen::Vector3f *center, float radius, Eigen::Vector3f *axis, 
               float reflect, const char *texture) {
    center_ = *center;
    R_ = radius;
    has_shadow_ = true;
    reflect_ = reflect;

    ty_ = *axis;
    ty_ *= (1.0f / ty_.norm());
    Eigen::Vector3f tmp = generate_unit_vector(&ty_);
    tx_ = tmp.cross(ty_);
    tx_ *= (1.0f / tx_.norm());
    tz_ = ty_.cross(tx_);
    tz_ *= (1.0f / tz_.norm());

    texture_ = textureCollector.add(texture);
}


Sphere::~Sphere() {
}


float Sphere::solve(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                    float mind, float maxd) {

    Eigen::Vector3f t = (*origin) - center_;
    float a = direction->dot(*direction);
    float b = 2.0f * direction->dot(t);
    float c = t.dot(t) - (R_ * R_);

    return solve_quadratic(a, b, c, mind, maxd);
}


Eigen::Vector3f Sphere::calculate_normal(Eigen::Vector3f *hit) {
    Eigen::Vector3f normal = (*hit) - center_;
    return (normal * (1.0f / normal.norm()));
}


/*
Guidelines:
https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html
*/
Pixel Sphere::pick_pixel(Eigen::Vector3f *hit, Eigen::Vector3f *normal) {
    float dot = normal->dot(ty_);
    float phi = std::acos(-dot);
    float fracy = phi / M_PI;

    dot = normal->dot(tx_);
    float theta = std::acos(dot / std::sin(phi)) / (2.0f * M_PI);
    dot = normal->dot(tz_);
    float fracx = (dot > 0.0f) ? theta : (1.0f - theta);

    return texture_->pick_pixel(fracx, fracy, 1.0f);
}


Cylinder::Cylinder(Eigen::Vector3f *center, Eigen::Vector3f *direction,
                   float radius, float span, float reflect,
                   const char *texture) {

    A_ = *center;
    B_ = *direction;
    B_ *= (1.0f / B_.norm());
    R_ = radius;

    span_ = span;
    reflect_ = reflect;
    has_shadow_ = true;

    ty_ = generate_unit_vector(&B_);
    tx_ = ty_.cross(B_);
    tx_ *= (1.0f / tx_.norm());

    texture_ = textureCollector.add(texture);
}


Cylinder::~Cylinder() {
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
float Cylinder::solve(Eigen::Vector3f *O, Eigen::Vector3f *D,
                      float mind, float maxd) {

    Eigen::Vector3f tmp = (*O) - A_;

    float a = D->dot(tmp);
    float b = D->dot(B_);
    float d = tmp.dot(B_);
    float f = (R_ * R_) - tmp.dot(tmp);

    // Solving a quadratic equation for t
    float aa = 1.0f - (b * b);
    float bb = 2.0f * (a - b * d);
    float cc = -(d * d) - f;
    float t = solve_quadratic(aa, bb, cc, mind, maxd);

    if (t > 0.0f) {
        // Check if the cylinder is finite
        if (span_ > 0.0f) {
            float alpha = d + t * b;
            if ((alpha < -span_) || (alpha > span_)) {
                return -1.0f;
            }
        }
    }
    return t;
}


Eigen::Vector3f Cylinder::calculate_normal(Eigen::Vector3f *hit) {
    // N = Hit - [B . (Hit - A)] * B
    Eigen::Vector3f tmp = (*hit) - A_;
    float alpha = B_.dot(tmp);
    Eigen::Vector3f bar = A_ + alpha * B_;
    Eigen::Vector3f normal = (*hit) - bar;

    return (normal * (1.0f / normal.norm()));
}


Pixel Cylinder::pick_pixel(Eigen::Vector3f *hit, Eigen::Vector3f *normal) {
    Eigen::Vector3f tmp = (*hit) - A_;
    float alpha = tmp.dot(B_);
    float dot = normal->dot(tx_);
    float fracx = acos(dot) / M_PI;
    float fracy = alpha / (2.0f * M_PI * R_);

    return texture_->pick_pixel(fracx, fracy, 1.0f);
}

}  // namespace mrtp

