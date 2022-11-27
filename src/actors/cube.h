#ifndef CUBE_H
#define CUBE_H

#include <memory>
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_cube(TextureFactory*, std::shared_ptr<ConfigTable>, std::vector<std::shared_ptr<ActorBase>>*); 

}

#endif
