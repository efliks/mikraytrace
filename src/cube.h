#ifndef CUBE_H
#define CUBE_H

#include "shrptr.h"
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_cube(TextureFactory*, shared_ptr<ConfigTable>, std::vector<shared_ptr<ActorBase> >*); 

}

#endif
