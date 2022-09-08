#ifndef FACTORY_H
#define FACTORY_H

#include <vector>
#include <memory>

#include "common.h"
#include "actors.h"
#include "texture.h"
#include "cpptoml.h"


namespace mrtp {

void create_actors(ActorType, TextureFactory*, std::shared_ptr<cpptoml::table>,
                   std::vector<std::shared_ptr<ActorBase>>*);

}

#endif
