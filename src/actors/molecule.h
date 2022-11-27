#ifndef MOLECULE_H
#define MOLECULE_H

#include <memory>
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_molecule(TextureFactory*, std::shared_ptr<ConfigTable>, std::vector<std::shared_ptr<ActorBase>>*);

}

#endif
