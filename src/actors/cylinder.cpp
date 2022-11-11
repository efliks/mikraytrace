#include <Eigen/Geometry>

#include "logger.h"

#include "actors/tools.h"
#include "actors/cylinder.h"


namespace mrtp {

SimpleCylinder::SimpleCylinder(const StandardBasis& local_basis, double radius, 
        double length, std::shared_ptr<TextureMapper> texture_mapper_ptr) : 
    ActorBase(local_basis, texture_mapper_ptr), 
    radius_(radius), length_(length) 
{
}


bool SimpleCylinder::has_shadow() const {
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

double SimpleCylinder::solve_light_ray(const Vector3d& O, const Vector3d& D, 
        double min_dist, double max_dist) const
{
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


Vector3d SimpleCylinder::calculate_normal_at_hit(const Vector3d& hit) const
{
    // N = Hit - [B . (Hit - A)] * B
    Vector3d v = hit - local_basis_.o;
    
    double alpha = local_basis_.vk.dot(v);
    
    Vector3d w = local_basis_.o + alpha * local_basis_.vk;
    Vector3d normal = hit - w;

    return normal * (1 / normal.norm());
}


void create_cylinder(TextureFactory* texture_factory,
                     std::shared_ptr<ConfigTable> cylinder_items,
                     std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    Vector3d cylinder_center_vec = cylinder_items->get_vector("center");
    if (!cylinder_center_vec.size()) {
        LOG_ERROR("Error parsing cylinder center");
        return;
    }

    Vector3d cylinder_direction_vec = cylinder_items->get_vector("direction");
    if (!cylinder_direction_vec.size()) {
        LOG_ERROR("Error parsing cylinder direction");
        return;
    }

    double cylinder_span = cylinder_items->get_value("span", -1);
    double cylinder_radius = cylinder_items->get_value("radius", 1);

    Vector3d fill_vec = fill_vector(cylinder_direction_vec);

    Vector3d cylinder_vec_i = fill_vec.cross(cylinder_direction_vec);
    Vector3d cylinder_vec_j = cylinder_direction_vec.cross(cylinder_vec_i);

    cylinder_vec_i *= (1 / cylinder_vec_i.norm());
    cylinder_vec_j *= (1 / cylinder_vec_j.norm());
    cylinder_direction_vec *= (1 / cylinder_direction_vec.norm());

    StandardBasis cylinder_basis{
        cylinder_center_vec,
        cylinder_vec_i,
        cylinder_vec_j,
        cylinder_direction_vec
    };

    auto texture_mapper_ptr = create_texture_mapper(
                cylinder_items, ActorType::Cylinder, texture_factory);
    if (!texture_mapper_ptr) {
        return;
    }

    auto cylinder_ptr = std::shared_ptr<ActorBase>(new SimpleCylinder(
        cylinder_basis,
        cylinder_radius,
        cylinder_span,
        texture_mapper_ptr
    ));

    actor_ptrs->push_back(cylinder_ptr);
}


}
