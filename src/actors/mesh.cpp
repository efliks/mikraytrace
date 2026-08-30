#include <string>
#include <fstream>
#include <iostream>
#include <cstddef>
#include <cstring>

#include "vector3.h"

#ifdef USE_LIB3DS
#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/mesh.h>
#endif

#include "actors/mesh.h"
#include "actors/tools.h"
#include "actors/triangle.h"

namespace mrtp {

#ifdef USE_LIB3DS
class File3dsWrapper
{
public:
    File3dsWrapper(const std::string& filename)
    {
        libfile = lib3ds_file_load(filename.c_str());
    }

    ~File3dsWrapper()
    {
        if (libfile != NULL) {
            lib3ds_file_free(libfile);
        }
    }

    bool is_failed() const
    {
        return libfile == NULL;
    }

    Lib3dsFile *libfile;
};


static void load_node_r(Lib3dsFile* libfile,
                        Lib3dsNode* node,
                        std::vector<Vector3d>* vertex_list)
{
    Lib3dsNode* p = node->childs;
    while (p != NULL) {
        load_node_r(libfile, p, vertex_list);
        p = p->next;
    }

    std::string node_name(node->name);
    if (node->type != LIB3DS_OBJECT_NODE || node_name == "$$$DUMMY") {
        return;
    }

    if (!node->user.d) {
        Lib3dsMesh* mesh = lib3ds_file_mesh_by_name(libfile, node->name);
        if (mesh == NULL) {
            return;
        }

        for (unsigned p = 0; p < mesh->faces; p++) {
            Lib3dsFace* face = &mesh->faceL[p];

            for (int i = 0; i < 3; i++) {
                Vector3d V(static_cast<double>(mesh->pointL[face->points[i]].pos[0]),
                            static_cast<double>(mesh->pointL[face->points[i]].pos[1]),
                            static_cast<double>(mesh->pointL[face->points[i]].pos[2]));

                vertex_list->push_back(V);
            }
        }
    }
}


static int load_3ds_file(const std::string& filename, std::vector<Vector3d>* vertex_list)
{
    File3dsWrapper filewrap(filename);
    if (filewrap.is_failed()) {
        return 0;  // Error
    }

    Lib3dsNode* node = filewrap.libfile->nodes;
    while (node != NULL) {
        load_node_r(filewrap.libfile, node, vertex_list);
        node = node->next;
    }

    return 1;  // Success
}
#endif  // USE_LIB3DS

static void load_custom_file(const std::string& filename, std::vector<Vector3d>* vertex_list)
{
    struct TriangleFace
    {
        unsigned short a, b, c;
    };

    std::ifstream f(filename.c_str(), std::ios::binary);

    if (f.is_open()) {
        char filetag[4];
        f.read(filetag, 4);

        if (std::strncmp(filetag, "MF3D", 4) == 0) {
            unsigned short num_vertices;
            f.read(static_cast<char *>(static_cast<void *>(&num_vertices)), sizeof(unsigned short));

            unsigned short num_faces;
            f.read(static_cast<char *>(static_cast<void *>(&num_faces)), sizeof(unsigned short));

            std::vector<Vector3f> tmp_vertex_list;
            tmp_vertex_list.resize(num_vertices);
            f.read(static_cast<char *>(static_cast<void *>(tmp_vertex_list.data())), sizeof(Vector3f) * num_vertices);

            std::vector<TriangleFace> faces_list;
            faces_list.resize(num_faces);
            f.read(static_cast<char *>(static_cast<void *>(faces_list.data())), sizeof(TriangleFace) * num_faces);

            f.close();

            for (std::vector<TriangleFace>::const_iterator it = faces_list.begin();
                 it != faces_list.end(); ++it) {
                Vector3f v = tmp_vertex_list[it->a];
                vertex_list->push_back(v.to_vector3());

                v = tmp_vertex_list[it->b];
                vertex_list->push_back(v.to_vector3());

                v = tmp_vertex_list[it->c];
                vertex_list->push_back(v.to_vector3());
            }

        }
    }
}


void create_mesh(TextureFactory* texture_factory,
                 shared_ptr<ConfigTable> items,
                 std::vector<shared_ptr<ActorBase> >* actor_ptrs)
{
    std::string filename = items->get_text("file3ds");
    if (filename.empty()) {
        std::cerr << "ERROR: Undefined mesh file" << std::endl;
        return;
    }

    Vector3d mesh_vec_o = items->get_vector("center");
    if (!mesh_vec_o.size()) {
        std::cerr << "ERROR: Error parsing mesh center" << std::endl;
        return;
    }

    shared_ptr<TextureMapper> texture_mapper_ptr = create_dummy_mapper(
                items, "color", "reflect");
    if (!texture_mapper_ptr) {
        return;
    }

    size_t idx = filename.rfind(".");
    std::string ext = filename.substr(idx + 1, filename.length() - idx - 1);
    std::vector<Vector3d> vertex_list;

    if (ext == "3d") {
        load_custom_file(filename, &vertex_list);
    }
#ifdef USE_LIB3DS
    else if (ext == "3ds") {
        if (!load_3ds_file(filename, &vertex_list)) {
            std::cerr << "ERROR: Error reading mesh file" << std::endl;
            return;
        }
    }
#endif  // USE_LIB3DS
    else {
        std::cerr << "ERROR: Unknown file extension " << ext << std::endl;
        return;
    }

    if (vertex_list.empty()) {
        std::cerr << "ERROR: No triangles found" << std::endl;
        return;
    }

    // Translate model to 0, 0, 0
    Vector3d vec_o;

    for (std::vector<Vector3d>::const_iterator it = vertex_list.begin(); it != vertex_list.end(); ++it) {
        vec_o += *it;
    }
    vec_o /= vertex_list.size();

    for (std::vector<Vector3d>::iterator it = vertex_list.begin(); it != vertex_list.end(); ++it) {
        *it -= vec_o;
    }

    // Normalize model
    double max_d = 0;

    for (std::vector<Vector3d>::const_iterator it = vertex_list.begin(); it != vertex_list.end(); ++it) {
        double d = it->norm();
        if (d > max_d) {
            max_d = d;
        }
    }

    for (std::vector<Vector3d>::iterator it = vertex_list.begin(); it != vertex_list.end(); ++it) {
        *it /= max_d;
    }

    // Rotate, scale, and translate model to center
    Matrix3d m_rot = create_rotation_matrix(items);

    double mesh_scale = items->get_value("scale", 1);

    for (std::vector<Vector3d>::iterator it = vertex_list.begin(); it != vertex_list.end(); ++it) {
        *it = mesh_scale * (m_rot * *it) + mesh_vec_o;
    }

    // Create triangles
    for (size_t i = 0; i < vertex_list.size() / 3; i++) {
        Vector3d A(vertex_list[i * 3]);
        Vector3d B(vertex_list[i * 3 + 1]);
        Vector3d C(vertex_list[i * 3 + 2]);

        Vector3d vec_o = (A + B + C) / 3;
        Vector3d vec_i = B - A;
        Vector3d vec_k = vec_i.cross(C - B);
        Vector3d vec_j = vec_k.cross(vec_i);

        vec_i *= (1 / vec_i.norm());
        vec_j *= (1 / vec_j.norm());
        vec_k *= (1 / vec_k.norm());

        StandardBasis local_basis;
        set_basis(&local_basis, vec_o, vec_i, vec_j, vec_k);

        shared_ptr<ActorBase> triangle_ptr = shared_ptr<ActorBase>(
                    new SimpleTriangle(local_basis, A, B, C, texture_mapper_ptr));

        actor_ptrs->push_back(triangle_ptr);
    }
}


}
