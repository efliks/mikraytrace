#ifndef MESH_H
#define MESH_H

#include <memory>
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_mesh(TextureFactory*, std::shared_ptr<ConfigTable>,
        std::vector<std::shared_ptr<ActorBase>>*);

}

#endif // MESH_H
