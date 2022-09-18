#ifndef BANNER_H
#define BANNER_H

#include <memory>
#include <vector>

#include "config.h"
#include "actors.h"
#include "texture.h"


namespace mrtp {

void create_banner(TextureFactory*, std::shared_ptr<ConfigTable>,
        std::vector<std::shared_ptr<ActorBase>>*);

}

#endif // BANNER_H
