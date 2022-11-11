#include <string>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/mesh.h>

#include "logger.h"

#include "actors/mesh.h"
#include "actors/tools.h"
#include "actors/triangle.h"

using Vector3d = Eigen::Vector3d;


namespace mrtp {

class File3dsWrapper
{
public:
    File3dsWrapper(const std::string& filename)
    {
        libfile = lib3ds_file_load(filename.c_str());
    }

    ~File3dsWrapper()
    {
        if (libfile != nullptr) {
            lib3ds_file_free(libfile);
        }
    }

    bool is_failed() const
    {
        return libfile == nullptr;
    }

    Lib3dsFile *libfile;
};


static void load_node_r(Lib3dsFile* libfile,
                        Lib3dsNode* node,
                        std::vector<Vector3d>* vertex_list)
{
    Lib3dsNode* p = node->childs;
    while (p != nullptr) {
        load_node_r(libfile, p, vertex_list);
        p = p->next;
    }

    std::string node_name(node->name);
    if (node->type != LIB3DS_OBJECT_NODE || node_name == "$$$DUMMY") {
        return;
    }

    if (!node->user.d) {
        Lib3dsMesh* mesh = lib3ds_file_mesh_by_name(libfile, node->name);
        if (mesh == nullptr) {
            return;
        }

        for (unsigned p = 0; p < mesh->faces; p++) {
            Lib3dsFace* face = &mesh->faceL[p];

            for (int i = 0; i < 3; i++) {
                Vector3d V{ static_cast<double>(mesh->pointL[face->points[i]].pos[0]),
                            static_cast<double>(mesh->pointL[face->points[i]].pos[1]),
                            static_cast<double>(mesh->pointL[face->points[i]].pos[2]) };

                vertex_list->push_back(V);
            }
        }
    }
}


void create_mesh(TextureFactory* texture_factory,
                 std::shared_ptr<ConfigTable> items,
                 std::vector<std::shared_ptr<ActorBase>>* actor_ptrs)
{
    std::string filename = items->get_text("file3ds");
    if (filename.empty()) {
        LOG_ERROR("Undefined mesh file");
        return;
    }

    File3dsWrapper filewrap(filename);
    if (filewrap.is_failed()) {
        LOG_ERROR("Cannot read mesh file");
        return;
    }

    Vector3d mesh_vec_o = items->get_vector("center");
    if (!mesh_vec_o.size()) {
        LOG_ERROR("Error parsing mesh center");
        return;
    }

    std::shared_ptr<TextureMapper> texture_mapper_ptr = create_dummy_mapper(
                items, "color", "reflect");
    if (!texture_mapper_ptr) {
        return;
    }

    std::vector<Vector3d> vertex_list;
    Lib3dsNode* node = filewrap.libfile->nodes;
    while (node != nullptr) {
        load_node_r(filewrap.libfile, node, &vertex_list);
        node = node->next;
    }

    if (vertex_list.empty()) {
        LOG_ERROR("No triangles found");
        return;
    }

    // Translate model to 0, 0, 0
    Vector3d vec_o;

    for (const Vector3d& v : vertex_list) {
        vec_o += v;
    }
    vec_o /= vertex_list.size();

    for (Vector3d& v : vertex_list) {
        v -= vec_o;
    }

    // Normalize model
    double max_d = 0;

    for (const Vector3d& v : vertex_list) {
        double d = v.norm();
        if (d > max_d) {
            max_d = d;
        }
    }

    for (Vector3d& v : vertex_list) {
        v /= max_d;
    }

    // Rotate, scale, and translate model to center
    Eigen::Matrix3d m_rot = create_rotation_matrix(items);

    double mesh_scale = items->get_value("scale", 1);

    for (Vector3d& v : vertex_list) {
        v = mesh_scale * (m_rot * v) + mesh_vec_o;
    }

    // Create triangles
    for (size_t i = 0; i < vertex_list.size() / 3; i++) {
        Vector3d A{vertex_list[i * 3]};
        Vector3d B{vertex_list[i * 3 + 1]};
        Vector3d C{vertex_list[i * 3 + 2]};

        Vector3d vec_o = (A + B + C) / 3;
        Vector3d vec_i = B - A;
        Vector3d vec_k = vec_i.cross(C - B);
        Vector3d vec_j = vec_k.cross(vec_i);

        vec_i *= (1 / vec_i.norm());
        vec_j *= (1 / vec_j.norm());
        vec_k *= (1 / vec_k.norm());

        StandardBasis local_basis{vec_o, vec_i, vec_j, vec_k};

        std::shared_ptr<ActorBase> triangle_ptr = std::shared_ptr<ActorBase>(
                    new SimpleTriangle(local_basis, A, B, C, texture_mapper_ptr));

        actor_ptrs->push_back(triangle_ptr);
    }
}


}
