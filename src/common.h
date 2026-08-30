#ifndef COMMON_H
#define COMMON_H

#include "vector3.h"


namespace mrtp {

struct StandardBasis
{
    Vector3d o { 0, 0, 0 };
    Vector3d vi { 1, 0, 0 };
    Vector3d vj { 0, 1, 0 };
    Vector3d vk { 0, 0, 1 };
};

enum class ActorType 
{
    Plane,
    Sphere,
    Cylinder,
    Triangle,
    Cube,
    Molecule,
    Banner,
    Mesh
};

}

#endif // COMMON_H
