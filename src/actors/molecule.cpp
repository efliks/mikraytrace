#include <Eigen/Geometry>

#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/obiter.h>
#include <openbabel/obconversion.h>

#include "actors/molecule.h"
#include "actors/cylinder.h"
#include "actors/sphere.h"
#include "actors/tools.h"

#include "logger.h"


namespace mrtp {

static void create_tables(const std::string& mol2file,
                          std::vector<unsigned int>* atomic_nums,
                          std::vector<Eigen::Vector3d>* positions,
                          std::vector<std::pair<unsigned int, unsigned int>>* bonds)
{
    OpenBabel::OBMol mol;
    OpenBabel::OBConversion conv;

    if(!conv.SetInFormat("mol2") || !conv.ReadFile(&mol, mol2file)) {
        return;
    }

    FOR_ATOMS_OF_MOL(a, mol) {
        atomic_nums->push_back(a->GetAtomicNum());
        positions->push_back(Eigen::Vector3d{a->GetX(), a->GetY(), a->GetZ()});
    }

    FOR_BONDS_OF_MOL(b, mol) {
        bonds->push_back(std::pair<unsigned int, unsigned int>{
                    b->GetBeginAtomIdx() - 1, b->GetEndAtomIdx() - 1});
    }
}


void create_molecule(TextureFactory* texture_factory,
                     std::shared_ptr<ConfigTable> items,
                     std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) 
{
    std::string mol2file_str = items->get_text("mol2file");
    if (mol2file_str.empty()) {
        LOG_ERROR("Undefined mol2 file");
        return;
    }

    std::fstream check(mol2file_str.c_str());
    if (!check.good()) {
        LOG_ERROR(std::string("Cannot open mol2 file " + mol2file_str));
        return;
    }

    std::vector<unsigned int> atomic_nums;
    std::vector<Vector3d> positions;
    std::vector<std::pair<unsigned int, unsigned int>> bonds;

    create_tables(mol2file_str, &atomic_nums, &positions, &bonds);

    if (atomic_nums.empty() || positions.empty() || bonds.empty()) {
        LOG_ERROR("Cannot create molecule");
        return;
    }

    Vector3d mol_vec_o = items->get_vector("center");
    if (!mol_vec_o.size()) {
        LOG_ERROR("Error parsing molecule center");
        return;
    }

    double mol_scale = items->get_value("scale", 1);
    double sphere_scale = items->get_value("atom_scale", 1);
    double cylinder_scale = items->get_value("bond_scale", 0.5);

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
