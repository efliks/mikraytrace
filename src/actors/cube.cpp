#include <Eigen/Geometry>
#include <easylogging++.h>

#include "actors/tools.h"
#include "actors/cube.h"
#include "actors/triangle.h"


namespace mrtp {

static void create_cube_triangles(double s,
                                  const StandardBasis& face_basis,
                                  std::shared_ptr<TextureMapper> texture_mapper_ptr,
                                  std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    Vector3d ta_A = face_basis.o + face_basis.vi * s + face_basis.vj * s;
    Vector3d ta_B = face_basis.o - face_basis.vi * s + face_basis.vj * s;
    Vector3d ta_C = face_basis.o + face_basis.vi * s - face_basis.vj * s;

    Vector3d tb_A = face_basis.o - face_basis.vi * s - face_basis.vj * s;
    Vector3d tb_B = face_basis.o + face_basis.vi * s - face_basis.vj * s;
    Vector3d tb_C = face_basis.o - face_basis.vi * s + face_basis.vj * s;

    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimpleTriangle(face_basis, ta_A, ta_B, ta_C, texture_mapper_ptr)));

    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimpleTriangle(face_basis, tb_A, tb_B, tb_C, texture_mapper_ptr)));
}


void create_cube(TextureFactory* texture_factory,
                 std::shared_ptr<ConfigTable> cube_items,
                 std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    Vector3d cube_vec_o = cube_items->get_vector("center");
    if (!cube_vec_o.size()) {
        LOG(ERROR) << "Error parsing cube center";
        return;
    }

    Vector3d cube_vec_k = cube_items->get_vector("direction", Vector3d{0, 0, 1});

    double cube_scale = cube_items->get_value("scale", 1) / 2;

    auto texture_mapper_ptr = create_dummy_mapper(cube_items, "color", "reflect");
    if (!texture_mapper_ptr) {
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

    create_cube_triangles(cube_scale, face_a_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_b_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_c_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_d_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_e_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_f_basis, texture_mapper_ptr, actor_ptrs);
}


}
