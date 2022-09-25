#include <string>

#include <Eigen/Geometry>
#include <easylogging++.h>

#include "actors/banner.h"
#include "actors/sphere.h"
#include "actors/tools.h"

#include "common.h"
#include "mappers.h"


namespace mrtp {

struct CubeFace
{
    int va;
    int vb;
    int vc;
};

struct CubeVertex
{
    int a;
    int b;
    int c;
};


extern const CubeFace kCubeFaces[];
extern const CubeVertex kCubeVertices[];
extern const unsigned char kCharacterData[];


static void create_char3d(char c,
                          double char_scale,
                          const StandardBasis& char_basis,
                          std::shared_ptr<TextureMapper> texture_mapper,
                          std::vector<std::shared_ptr<ActorBase>>* actor_ptrs)
{
    if (c > 'a' && c < 'z') {
        c += ('A' - 'a');
    }

    size_t idx = static_cast<size_t>((c - 32) & 63);
    const unsigned char* cptr = &kCharacterData[idx << 3];

    for (int i = 0; i < 8; i++, cptr++) {
        for (int j = 7; j >= 0; j--) {
            if ((*cptr >> j) & 1) {
                // put a sphere
                double block_scale = char_scale / 8;

                Vector3d o_vec = ( (7 - j) * block_scale - (8 * block_scale / 2) + block_scale / 2 ) * char_basis.vj + ( (7 - i) * block_scale - (8 * block_scale / 2) + block_scale / 2 ) * char_basis.vk + char_basis.o;

                StandardBasis sphere_basis;
                sphere_basis.o = o_vec;

                auto sphere_ptr = std::shared_ptr<ActorBase>(
                    new SimpleSphere(sphere_basis, char_scale / 8 / 2, texture_mapper));

                actor_ptrs->push_back(sphere_ptr);
            }
        }
    }
}


void create_banner(TextureFactory* texture_factory,
                   std::shared_ptr<ConfigTable> items,
                   std::vector<std::shared_ptr<ActorBase>>* actor_ptrs)
{
    std::string banner_text = items->get_text("text");
    if (banner_text.empty()) {
        LOG(ERROR) << "Error parsing banner text";
        return;
    }

    std::shared_ptr<TextureMapper> banner_mapper =
            create_dummy_mapper(items, "color", "reflect");
    if (!banner_mapper) {
        return;
    }

    Vector3d banner_o_vec = items->get_vector("center");
    if (!banner_o_vec.size()) {
        LOG(ERROR) << "Error parsing banner center";
        return;
    }

    Eigen::Matrix3d m_rot = create_rotation_matrix(items);

    Vector3d banner_i_vec{m_rot.col(0)};
    Vector3d banner_j_vec{m_rot.col(1)};
    Vector3d banner_k_vec{m_rot.col(2)};

    banner_i_vec *= (1 / banner_i_vec.norm());
    banner_j_vec *= (1 / banner_j_vec.norm());
    banner_k_vec *= (1 / banner_k_vec.norm());

    // Each char can rotate individually
    Eigen::Matrix3d m_char_rot = create_rotation_matrix(items, "char_");

    Vector3d char_i_vec = m_char_rot * banner_i_vec;
    Vector3d char_j_vec = m_char_rot * banner_j_vec;
    Vector3d char_k_vec = m_char_rot * banner_k_vec;

    double char_scale = items->get_value("scale", 1);
    int char_idx = 0;

    for (char c : banner_text) {
        Vector3d char_o_vec = banner_o_vec + (char_scale * char_idx - (banner_text.size() - 1) * char_scale / 2) * banner_j_vec;
        char_idx++;

        StandardBasis char_basis{char_o_vec, char_i_vec, char_j_vec, char_k_vec};

        create_char3d(c, char_scale, char_basis, banner_mapper, actor_ptrs);
    }
}


const CubeFace kCubeFaces[] = {{0, 1, 2}, {0, 2, 3}, {1, 5, 6}, {1, 6, 2}, {4, 0, 3}, {4, 3, 7}, {4, 5, 1}, {4, 1, 0}, {3, 2, 6}, {3, 6, 7}, {5, 4, 7}, {5, 7, 6}};

const CubeVertex kCubeVertices[] = {{-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}, {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}};

const unsigned char kCharacterData[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x08, 0x00, 0x18, 0x00, 0x28, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x7c, 0x28, 0x28, 0x28, 0x7c, 0x28, 0x00, 0x10, 0x7e, 0x40, 0x7e, 0x86, 0xfe, 0x10, 0x00, 0xe2, 0xa4, 0xe8, 0x10, 0x2e, 0x4a, 0x8e, 0x00, 0x38, 0x28, 0x7c, 0x44, 0xc4, 0xce, 0xfc, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x0c, 0x08, 0x0c, 0x04, 0x02, 0x00, 0x80, 0x40, 0x60, 0x20, 0x60, 0x40, 0x80, 0x00, 0x00, 0x54, 0x38, 0x6c, 0x38, 0x54, 0x00, 0x00, 0x00, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x04, 0x04, 0x08, 0x10, 0x20, 0x40, 0x40, 0x00, 0x7e, 0x42, 0x42, 0xc2, 0xc6, 0xc6, 0xfe, 0x00, 0x38, 0x00, 0x08, 0x18, 0x18, 0x18, 0x18, 0x00, 0x7c, 0x04, 0x7c, 0xc0, 0xc0, 0xc6, 0xfe, 0x00, 0x7e, 0x02, 0x1e, 0x06, 0x06, 0xc6, 0xfe, 0x00, 0x26, 0x22, 0x3e, 0x06, 0x06, 0x06, 0x06, 0x00, 0x7c, 0x40, 0x7c, 0x06, 0x06, 0xc6, 0xfe, 0x00, 0x78, 0x40, 0x7e, 0x46, 0xc6, 0xc6, 0xfe, 0x00, 0xfe, 0x86, 0x0c, 0x0c, 0x18, 0x18, 0x18, 0x00, 0x3c, 0x24, 0x7e, 0x42, 0xc2, 0xc6, 0xfe, 0x00, 0x7e, 0x42, 0x7e, 0x02, 0x06, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x08, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x1c, 0x36, 0x66, 0x0c, 0x18, 0x00, 0x18, 0x00, 0x7c, 0x42, 0x1a, 0x2a, 0x2a, 0x2a, 0x1c, 0x00, 0x3c, 0x26, 0x7e, 0x42, 0xc2, 0xc6, 0xc6, 0x00, 0x78, 0x4c, 0x78, 0xcc, 0xc6, 0xc6, 0xfc, 0x00, 0x7e, 0x42, 0xc0, 0xc0, 0xc0, 0xc6, 0xfe, 0x00, 0x78, 0x4c, 0x42, 0xc2, 0xc6, 0xce, 0xfc, 0x00, 0x7e, 0x42, 0x40, 0xf0, 0xc0, 0xc6, 0xfe, 0x00, 0x7e, 0x42, 0x40, 0xf0, 0xc0, 0xc0, 0xc0, 0x00, 0x7e, 0x46, 0x40, 0xc0, 0xce, 0xc6, 0xfe, 0x00, 0x42, 0x42, 0x7e, 0xc2, 0xc2, 0xc2, 0xc6, 0x00, 0x08, 0x08, 0x08, 0x18, 0x18, 0x18, 0x18, 0x00, 0x02, 0x06, 0x06, 0x06, 0xc6, 0xc6, 0xfe, 0x00, 0x48, 0x50, 0x70, 0xd8, 0xcc, 0xc6, 0xc6, 0x00, 0x40, 0x40, 0x40, 0xc0, 0xc0, 0xc6, 0xfe, 0x00, 0x42, 0x66, 0x7e, 0xda, 0xc2, 0xc2, 0xc6, 0x00, 0x62, 0x62, 0x52, 0xda, 0xce, 0xc6, 0xc6, 0x00, 0x7e, 0x42, 0x42, 0xc2, 0xc6, 0xc6, 0xfe, 0x00, 0x7c, 0x42, 0x42, 0xfc, 0xc0, 0xc0, 0xc0, 0x00, 0x7e, 0x42, 0x42, 0xc2, 0xce, 0xc6, 0xfe, 0x00, 0x7c, 0x46, 0x7c, 0xd8, 0xc8, 0xce, 0xc6, 0x00, 0x7e, 0x40, 0x7e, 0x06, 0x06, 0xc6, 0xfe, 0x00, 0x7e, 0x08, 0x08, 0x18, 0x18, 0x18, 0x18, 0x00, 0x46, 0x46, 0xc2, 0xc2, 0xc2, 0xc2, 0xfe, 0x00, 0x46, 0x46, 0x62, 0x32, 0x36, 0x1c, 0x1c, 0x00, 0x46, 0x46, 0xc2, 0xd2, 0xfe, 0xee, 0xc6, 0x00, 0xc6, 0x6c, 0x38, 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x46, 0x46, 0x7c, 0x38, 0x18, 0x18, 0x18, 0x00, 0x7e, 0x06, 0x3c, 0x30, 0xe0, 0xe6, 0xfe, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00, 0x40, 0x40, 0x20, 0x10, 0x08, 0x04, 0x04, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x10, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

}
