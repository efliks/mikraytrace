#include <Eigen/Geometry>

#include "actors/triangle.h"
#include "actors/plane.h"

#include "logger.h"


namespace mrtp {

SimpleTriangle::SimpleTriangle(const StandardBasis& local_basis, 
        const Vector3d& A, const Vector3d& B, const Vector3d& C, 
        std::shared_ptr<TextureMapper> texture_mapper_ptr) : 
    ActorBase(local_basis, texture_mapper_ptr), 
    A_(A), B_(B), C_(C) 
{
    TA_ = local_basis_.vk.cross(A - C);
    TB_ = local_basis_.vk.cross(B - A);
    TC_ = local_basis_.vk.cross(C - B);
}


bool SimpleTriangle::has_shadow() const {
    return true;
}


Vector3d SimpleTriangle::calculate_normal_at_hit(const Vector3d& hit) const {
    return local_basis_.vk;
}


double SimpleTriangle::solve_light_ray(const Vector3d& O, const Vector3d& D, 
        double min_dist, double max_dist) const
{
    SimplePlane plane(local_basis_, texture_mapper_);
    double t = plane.solve_light_ray(O, D, min_dist, max_dist);
    
    if (t > 0) {
        Vector3d X = O + t * D;
        if ((X - A_).dot(TA_) > 0 &&
            (X - B_).dot(TB_) > 0 &&
            (X - C_).dot(TC_) > 0) {
            return t;
        }
    }
    
    return -1;
}


void create_triangle(TextureFactory* texture_factory,
                     std::shared_ptr<ConfigTable> items,
                     std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    Vector3d A = items->get_vector("A");
    if (!A.size()) {
        LOG_ERROR("Error parsing vertex A in triangle");
        return;
    }

    Vector3d B = items->get_vector("B");
    if (!B.size()) {
        LOG_ERROR("Error parsing vertex B in triangle");
        return;
    }

    Vector3d C = items->get_vector("C");
    if (!C.size()) {
        LOG_ERROR("Error parsing vertex C in triangle");
        return;
    }

    Vector3d vec_o = (A + B + C) / 3;
    Vector3d vec_i = B - A;
    Vector3d vec_k = vec_i.cross(C - B);
    Vector3d vec_j = vec_k.cross(vec_i);

    vec_i *= (1 / vec_i.norm());
    vec_j *= (1 / vec_j.norm());
    vec_k *= (1 / vec_k.norm());

    StandardBasis local_basis{vec_o, vec_i, vec_j, vec_k};

    auto texture_mapper_ptr = create_dummy_mapper(items, "color", "reflect");
    if (!texture_mapper_ptr) {
        return;
    }

    auto new_triangle_ptr = std::shared_ptr<ActorBase>(
                new SimpleTriangle(local_basis, A, B, C, texture_mapper_ptr));

    actor_ptrs->push_back(new_triangle_ptr);
}


}
