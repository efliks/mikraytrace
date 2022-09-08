#include <Eigen/Geometry>
#include <easylogging++.h>

#include "actors/sphere.h"
#include "actors/tools.h"


namespace mrtp {

SimpleSphere::SimpleSphere(const StandardBasis& local_basis, double radius, 
        std::shared_ptr<TextureMapper> texture_mapper_ptr) : 
    ActorBase(local_basis, texture_mapper_ptr), 
    radius_(radius) {

}


bool SimpleSphere::has_shadow() const {
    return true;
}


Vector3d SimpleSphere::calculate_normal_at_hit(const Vector3d& hit) const {
    Vector3d t = hit - local_basis_.o;
    return t * (1 / t.norm());
}


double SimpleSphere::solve_light_ray(const Vector3d& O, const Vector3d& D, 
        double min_dist, double max_dist) const 
{
    Vector3d t = O - local_basis_.o;

    double a = D.dot(D);
    double b = 2 * D.dot(t);
    double c = t.dot(t) - radius_ * radius_;
    double d = solve_quadratic(a, b, c);
    
    if (d > min_dist && d < max_dist) {
        return d;
    }
    return -1;
}


void create_sphere(TextureFactory* texture_factory,
                   std::shared_ptr<cpptoml::table> sphere_items,
                   std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    auto sphere_center = sphere_items->get_array_of<double>("center");
    if (!sphere_center) {
        LOG(ERROR) << "Error parsing sphere center";
        return;
    }
    Vector3d sphere_center_vec(sphere_center->data());

    Vector3d sphere_axis_vec(0, 0, 1);
    auto sphere_axis = sphere_items->get_array_of<double>("axis");
    if (sphere_axis) {
        Vector3d tmp_vec(sphere_axis->data());
        sphere_axis_vec = tmp_vec;
    }

    double sphere_radius = sphere_items->get_as<double>("radius").value_or(1);

    Vector3d fill_vec = fill_vector(sphere_axis_vec);

    Vector3d sphere_vec_i = fill_vec.cross(sphere_axis_vec);
    Vector3d sphere_vec_j = sphere_axis_vec.cross(sphere_vec_i);

    sphere_vec_i *= (1 / sphere_vec_i.norm());
    sphere_vec_j *= (1 / sphere_vec_j.norm());
    sphere_axis_vec *= (1 / sphere_axis_vec.norm());

    StandardBasis sphere_basis{
        sphere_center_vec,
        sphere_vec_i,
        sphere_vec_j,
        sphere_axis_vec
    };

    auto texture_mapper_ptr = create_texture_mapper(
                sphere_items, ActorType::Sphere, texture_factory);
    if (!texture_mapper_ptr) {
        return;
    }

    auto sphere_ptr = std::shared_ptr<ActorBase>(
            new SimpleSphere(sphere_basis, sphere_radius, texture_mapper_ptr));

    actor_ptrs->push_back(sphere_ptr);
}


}
