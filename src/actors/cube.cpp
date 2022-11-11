#include <Eigen/Geometry>

#include "logger.h"

#include "actors/cube.h"
#include "actors/tools.h"
#include "actors/plane.h"
#include "actors/polygon.h"
#include "actors/triangle.h"


namespace mrtp {

void create_cube(TextureFactory* texture_factory,
                 std::shared_ptr<ConfigTable> cube_items,
                 std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    Vector3d cube_vec_o = cube_items->get_vector("center");
    if (!cube_vec_o.size()) {
        LOG_ERROR("Error parsing cube center");
        return;
    }

    Vector3d cube_vec_k = cube_items->get_vector("direction", Vector3d{0, 0, 1});

    double cube_scale = cube_items->get_value("scale", 1) / 2;

    auto texture_mapper = create_dummy_mapper(cube_items, "color", "reflect");
    if (!texture_mapper) {
        return;
    }

    Vector3d fill_vec = fill_vector(cube_vec_k);
    Vector3d cube_vec_i = fill_vec.cross(cube_vec_k);
    Vector3d cube_vec_j = cube_vec_k.cross(cube_vec_i);

    cube_vec_i *= (1 / cube_vec_i.norm());
    cube_vec_j *= (1 / cube_vec_j.norm());
    cube_vec_k *= (1 / cube_vec_k.norm());

    Eigen::Matrix3d m_rot = create_rotation_matrix(cube_items);

    cube_vec_i = m_rot * cube_vec_i;
    cube_vec_j = m_rot * cube_vec_j;
    cube_vec_k = m_rot * cube_vec_k;

    Vector3d face_a_o = cube_vec_k * cube_scale + cube_vec_o;
    Vector3d face_b_o = -cube_vec_i * cube_scale + cube_vec_o;
    Vector3d face_c_o = -cube_vec_k * cube_scale + cube_vec_o;
    Vector3d face_d_o = cube_vec_i * cube_scale + cube_vec_o;
    Vector3d face_e_o = cube_vec_j * cube_scale + cube_vec_o;
    Vector3d face_f_o = -cube_vec_j * cube_scale + cube_vec_o;

    StandardBasis face_a_basis{face_a_o, cube_vec_i, cube_vec_j, cube_vec_k};
    StandardBasis face_b_basis{face_b_o, cube_vec_k, cube_vec_j, -cube_vec_i};
    StandardBasis face_c_basis{face_c_o, -cube_vec_i, cube_vec_j, -cube_vec_k};
    StandardBasis face_d_basis{face_d_o, -cube_vec_k, cube_vec_j, cube_vec_i};
    StandardBasis face_e_basis{face_e_o, -cube_vec_k, -cube_vec_i, cube_vec_j};
    StandardBasis face_f_basis{face_f_o, -cube_vec_k, cube_vec_i, -cube_vec_j};

    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimplePolygon(face_a_basis, texture_mapper, cube_scale, cube_scale)));
    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimplePolygon(face_b_basis, texture_mapper, cube_scale, cube_scale)));
    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimplePolygon(face_c_basis, texture_mapper, cube_scale, cube_scale)));
    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimplePolygon(face_d_basis, texture_mapper, cube_scale, cube_scale)));
    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimplePolygon(face_e_basis, texture_mapper, cube_scale, cube_scale)));
    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimplePolygon(face_f_basis, texture_mapper, cube_scale, cube_scale)));
}


}
