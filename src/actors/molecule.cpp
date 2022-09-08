#include <Eigen/Geometry>
#include <easylogging++.h>

#include "actors/molecule.h"

#include "actors/cylinder.h"
#include "actors/sphere.h"
#include "actors/tools.h"

#include "babel.h"


namespace mrtp {

void create_molecule(TextureFactory* texture_factory,
                     std::shared_ptr<cpptoml::table> items,
                     std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    auto filename = items->get_as<std::string>("mol2file");
    if (!filename) {
        LOG(ERROR) << "Undefined mol2 file";
        return;
    }
    std::string mol2file_str(filename->data());

    std::fstream check(mol2file_str.c_str());
    if (!check.good()) {
        LOG(ERROR) << "Cannot open mol2 file " << mol2file_str;
        return;
    }

    std::vector<unsigned int> atomic_nums;
    std::vector<Vector3d> positions;
    std::vector<std::pair<unsigned int, unsigned int>> bonds;

    create_molecule_tables(mol2file_str, &atomic_nums, &positions, &bonds);

    if (atomic_nums.empty() || positions.empty() || bonds.empty()) {
        LOG(ERROR) << "Cannot create molecule";
        return;
    }

    auto mol_center = items->get_array_of<double>("center");
    if (!mol_center) {
        LOG(ERROR) << "Error parsing molecule center";
        return;
    }
    Vector3d mol_vec_o(mol_center->data());

    double mol_scale = items->get_as<double>("scale").value_or(1.0);
    double sphere_scale = items->get_as<double>("atom_scale").value_or(1.0);
    double cylinder_scale = items->get_as<double>("bond_scale").value_or(0.5);

    Eigen::Matrix3d m_rot = create_rotation_matrix(items);

    auto sphere_mapper_ptr = create_dummy_mapper(items, "atom_color", "atom_reflect");
    if (!sphere_mapper_ptr) {
        return;
    }

    auto cylinder_mapper_ptr = create_dummy_mapper(items, "bond_color", "bond_reflect");
    if (!cylinder_mapper_ptr) {
        return;
    }

    Vector3d center_vec{0, 0, 0};
    for (auto& atom_vec : positions) {
        center_vec += atom_vec;
    }
    center_vec *= (1. / positions.size());

    std::vector<Vector3d> transl_pos;
    for (auto& atom_vec : positions) {
        Vector3d transl_atom_vec = (m_rot * (atom_vec - center_vec)) * mol_scale + mol_vec_o;
        transl_pos.push_back(transl_atom_vec);
    }

    for (auto& atom_vec : transl_pos) {
        StandardBasis sphere_basis;
        sphere_basis.o = atom_vec;

        actor_ptrs->push_back(std::shared_ptr<ActorBase>(new SimpleSphere(
                sphere_basis, sphere_scale, sphere_mapper_ptr)));
    }

    for (auto& bond : bonds) {
        Vector3d cylinder_begin_vec = transl_pos[bond.first];
        Vector3d cylinder_end_vec = transl_pos[bond.second];

        Vector3d cylinder_center_vec = (cylinder_begin_vec + cylinder_end_vec) / 2;
        Vector3d cylinder_k_vec = cylinder_end_vec - cylinder_begin_vec;
        double cylinder_span = cylinder_k_vec.norm() / 2;

        Vector3d fill_vec = fill_vector(cylinder_k_vec);

        Vector3d cylinder_i_vec = fill_vec.cross(cylinder_k_vec);
        Vector3d cylinder_j_vec = cylinder_k_vec.cross(cylinder_i_vec);

        cylinder_i_vec *= (1 / cylinder_i_vec.norm());
        cylinder_j_vec *= (1 / cylinder_j_vec.norm());
        cylinder_k_vec *= (1 / cylinder_k_vec.norm());

        StandardBasis cylinder_basis{
            cylinder_center_vec,
            cylinder_i_vec,
            cylinder_j_vec,
            cylinder_k_vec
        };

        actor_ptrs->push_back(std::shared_ptr<ActorBase>(new SimpleCylinder(
                cylinder_basis, cylinder_scale, cylinder_span, cylinder_mapper_ptr)));
    }
}


}
