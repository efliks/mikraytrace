#ifndef BANNER_H
#define BANNER_H

#include "shrptr.h"
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_banner(TextureFactory*, shared_ptr<ConfigTable>, std::vector<shared_ptr<ActorBase> >*);

}

#endif // BANNER_H
