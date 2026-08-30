#ifndef COMMON_H
#define COMMON_H

#include "vector3.h"


namespace mrtp {

struct StandardBasis
{
    StandardBasis() : o(0, 0, 0), vi(1, 0, 0), vj(0, 1, 0), vk(0, 0, 1) {}

    Vector3d o;
    Vector3d vi;
    Vector3d vj;
    Vector3d vk;
};

enum ActorType
{
    ActorType_Plane,
    ActorType_Sphere,
    ActorType_Cylinder,
    ActorType_Triangle,
    ActorType_Cube,
    ActorType_Molecule,
    ActorType_Banner,
    ActorType_Mesh
};

}

#endif // COMMON_H
