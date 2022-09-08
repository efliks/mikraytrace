#ifndef MOLECULE_H
#define MOLECULE_H

#include <memory>
#include <vector>

#include "actors.h"
#include "texture.h"
#include "cpptoml.h"


namespace mrtp {

void create_molecule(TextureFactory*, std::shared_ptr<cpptoml::table>, 
        std::vector<std::shared_ptr<ActorBase>>*);


}

#endif
