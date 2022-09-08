#include <Eigen/Geometry>
#include <easylogging++.h>

#include "actors/plane.h"
#include "actors/tools.h"


namespace mrtp {

SimplePlane::SimplePlane(const StandardBasis& local_basis, 
        std::shared_ptr<TextureMapper> texture_mapper_ptr) : 
    ActorBase(local_basis, texture_mapper_ptr) {

}


bool SimplePlane::has_shadow() const {
    return false;
}


Vector3d SimplePlane::calculate_normal_at_hit(const Vector3d& hit) const {
    return local_basis_.vk;
}


double SimplePlane::solve_light_ray(const Vector3d& O, const Vector3d& D, 
        double min_dist, double max_dist) const 
{
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


void create_plane(TextureFactory* texture_factory,
                  std::shared_ptr<cpptoml::table> plane_items,
                  std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    auto plane_center = plane_items->get_array_of<double>("center");
    if (!plane_center) {
        LOG(ERROR) << "Error parsing plane center";
        return;
    }
    Vector3d plane_center_vec(plane_center->data());

    auto plane_normal = plane_items->get_array_of<double>("normal");
    if (!plane_normal) {
        LOG(ERROR) << "Error parsing plane normal";
        return;
    }
    Vector3d plane_normal_vec(plane_normal->data());

    Vector3d fill_vec = fill_vector(plane_normal_vec);

    Vector3d plane_vec_i = fill_vec.cross(plane_normal_vec);
    Vector3d plane_vec_j = plane_normal_vec.cross(plane_vec_i);

    plane_vec_i *= (1 / plane_vec_i.norm());
    plane_vec_j *= (1 / plane_vec_j.norm());
    plane_normal_vec *= (1 / plane_normal_vec.norm());

    StandardBasis plane_basis{
        plane_center_vec,
        plane_vec_i,
        plane_vec_j,
        plane_normal_vec
    };

    auto texture_mapper_ptr = create_texture_mapper(
            plane_items, ActorType::Plane, texture_factory);
    if (!texture_mapper_ptr) {
        return;
    }

    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                new SimplePlane(plane_basis, texture_mapper_ptr)));
}


}
