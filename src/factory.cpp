#include "factory.h"

#include "actors/cube.h"
#include "actors/cylinder.h"
#include "actors/molecule.h"
#include "actors/plane.h"
#include "actors/sphere.h"
#include "actors/triangle.h"


namespace mrtp {

void create_actors(ActorType actor_type,
                   TextureFactory* texture_factory,
                   std::shared_ptr<cpptoml::table> actor_items,
                   std::vector<std::shared_ptr<ActorBase>>* actor_ptrs)
{
    if (actor_type == ActorType::Plane)
        create_plane(texture_factory, actor_items, actor_ptrs);
    else if (actor_type == ActorType::Sphere)
        create_sphere(texture_factory, actor_items, actor_ptrs);
    else if (actor_type == ActorType::Cylinder)
        create_cylinder(texture_factory, actor_items, actor_ptrs);
    else if (actor_type == ActorType::Triangle)
        create_triangle(texture_factory, actor_items, actor_ptrs);
    else if (actor_type == ActorType::Cube)
        create_cube(texture_factory, actor_items, actor_ptrs);
    else if (actor_type == ActorType::Molecule)
        create_molecule(texture_factory, actor_items, actor_ptrs);
}


}
