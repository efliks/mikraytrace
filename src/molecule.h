#ifndef MOLECULE_H
#define MOLECULE_H

#include "shrptr.h"
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_molecule(TextureFactory*, shared_ptr<ConfigTable>, std::vector<shared_ptr<ActorBase> >*);

}

#endif
