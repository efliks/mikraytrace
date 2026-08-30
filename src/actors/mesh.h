#ifndef MESH_H
#define MESH_H

#include "shrptr.h"
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_mesh(TextureFactory*, shared_ptr<ConfigTable>, std::vector<shared_ptr<ActorBase> >*);

}

#endif // MESH_H
