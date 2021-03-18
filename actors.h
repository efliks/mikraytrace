/* File      : actors.h
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#pragma once

#include <Eigen/Core>

#include "pixel.h"
#include "texture.h"


namespace mrtp {

class Actor {
  public:
    Actor();

    bool has_shadow();
    float get_reflect();

    virtual ~Actor();
    virtual float solve(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                        float mind, float maxd) = 0;

    virtual Pixel pick_pixel(Eigen::Vector3f *hit,
                             Eigen::Vector3f *normal) = 0;

    virtual Eigen::Vector3f calculate_normal(Eigen::Vector3f *hit) = 0;

  protected:
    bool has_shadow_;
    float reflect_;
    Texture *texture_;
};


class Plane : public Actor {
  public:
    Plane(Eigen::Vector3f *center, Eigen::Vector3f *normal, float scale, 
          float reflect, const char *texture);

    virtual ~Plane();
    virtual float solve(Eigen::Vector3f *origin, Eigen::Vector3f *direction, 
                        float mind, float maxd) override;
   
    virtual Pixel pick_pixel(Eigen::Vector3f *hit,
                             Eigen::Vector3f *normal) override;
    
    virtual Eigen::Vector3f calculate_normal(Eigen::Vector3f *hit) override;

  private:
    Eigen::Vector3f center_;
    Eigen::Vector3f normal_;
    Eigen::Vector3f tx_;
    Eigen::Vector3f ty_;
    float scale_;
};


class Sphere : public Actor {
  public:
    Sphere(Eigen::Vector3f *center, float radius, Eigen::Vector3f *axis, 
           float reflect, const char *texture);

    virtual ~Sphere();
    virtual float solve(Eigen::Vector3f *origin, Eigen::Vector3f *direction, 
                        float mind, float maxd) override;

    virtual Pixel pick_pixel(Eigen::Vector3f *hit,
                             Eigen::Vector3f *normal) override;

    virtual Eigen::Vector3f calculate_normal(Eigen::Vector3f *hit) override;

  private:
    Eigen::Vector3f center_;
    Eigen::Vector3f tx_;
    Eigen::Vector3f ty_;
    Eigen::Vector3f tz_;
    float R_;
};


class Cylinder : public Actor {
  public:
    Cylinder(Eigen::Vector3f *center, Eigen::Vector3f *direction, 
             float radius, float span, float reflect, const char *texture);

    virtual ~Cylinder();
    virtual float solve(Eigen::Vector3f *origin, Eigen::Vector3f *direction, 
                        float mind, float maxd) override;

    virtual Pixel pick_pixel(Eigen::Vector3f *hit,
                             Eigen::Vector3f *normal) override;

    virtual Eigen::Vector3f calculate_normal(Eigen::Vector3f *hit) override;

  private:
    Eigen::Vector3f A_;
    Eigen::Vector3f B_;
    Eigen::Vector3f tx_;
    Eigen::Vector3f ty_;
    float R_;
    float span_;
};

}  // namespace mrtp
