#include <iostream>

#include "config.h"
#include "world.h"

#include "actors/mesh.h"
#include "actors/banner.h"
#include "actors/cube.h"
#include "actors/cylinder.h"
#include "actors/molecule.h"
#include "actors/plane.h"
#include "actors/sphere.h"
#include "actors/triangle.h"


namespace mrtp {

void SceneWorld::add_light(shared_ptr<Light> light_ptr) {
    light_ = light_ptr;
}


void SceneWorld::add_camera(shared_ptr<Camera> camera_ptr) {
    camera_ = camera_ptr;
}


void SceneWorld::add_actor(shared_ptr<ActorBase> actor_ptr) {
    actor_ptrs_.push_back(actor_ptr);
}


Light* SceneWorld::get_light_ptr() {
    return light_.get();  // FIXME
}


Camera* SceneWorld::get_camera_ptr() {
    return camera_.get();  // FIXME
}


ActorIterator SceneWorld::get_actor_iterator() {
    return ActorIterator(&actor_ptrs_);
}


ActorIterator::ActorIterator(std::vector<shared_ptr<ActorBase> >* actor_ptrs):
    actor_ptrs_(actor_ptrs) {
    actor_iter_ = actor_ptrs_->begin();
}


void ActorIterator::first() {
    actor_iter_ = actor_ptrs_->begin();
}


void ActorIterator::next() {
    actor_iter_++;
}


bool ActorIterator::is_done() {
    return actor_iter_ == actor_ptrs_->end();
}


std::vector<shared_ptr<ActorBase> >::iterator ActorIterator::current() {
    return actor_iter_;
}


class WorldBuilder {
public:
    WorldBuilder(const std::string& world_filename,
                 TextureFactory* texture_factory) :
        world_filename_(world_filename),
        texture_factory_(texture_factory) {

    }

    shared_ptr<SceneWorld> build() const
    {
        shared_ptr<ConfigReader> world_config = open_config(world_filename_);
        if (!world_config) {
            return shared_ptr<SceneWorld>();
        }

        std::vector<shared_ptr<ActorBase> > new_actors;

        shared_ptr<ConfigTableIterator> planes_array = world_config->get_tables("plane");
        process_actor_array(ActorType_Plane, planes_array, &new_actors);

        shared_ptr<ConfigTableIterator> spheres_array = world_config->get_tables("sphere");
        process_actor_array(ActorType_Sphere, spheres_array, &new_actors);

        shared_ptr<ConfigTableIterator> cylinders_array = world_config->get_tables("cylinder");
        process_actor_array(ActorType_Cylinder, cylinders_array, &new_actors);

        shared_ptr<ConfigTableIterator> triangles_array = world_config->get_tables("triangle");
        process_actor_array(ActorType_Triangle, triangles_array, &new_actors);

        shared_ptr<ConfigTableIterator> cubes_array = world_config->get_tables("cube");
        process_actor_array(ActorType_Cube, cubes_array, &new_actors);

        shared_ptr<ConfigTableIterator> molecules_array = world_config->get_tables("molecule");
        process_actor_array(ActorType_Molecule, molecules_array, &new_actors);

        shared_ptr<ConfigTableIterator> banners_array = world_config->get_tables("banner");
        process_actor_array(ActorType_Banner, banners_array, &new_actors);

        shared_ptr<ConfigTableIterator> meshes_array = world_config->get_tables("mesh");
        process_actor_array(ActorType_Mesh, meshes_array, &new_actors);

        if (new_actors.size() < 1) {
            std::cerr << "ERROR: No actors found" << std::endl;
            return shared_ptr<SceneWorld>();
        }

        shared_ptr<SceneWorld> world_ptr = shared_ptr<SceneWorld>(new SceneWorld());
        for (std::vector<shared_ptr<ActorBase> >::const_iterator it = new_actors.begin();
             it != new_actors.end(); ++it) {
            world_ptr->add_actor(*it);
        }

        shared_ptr<ConfigTable> camera_table = get_single_table(world_config, "camera");
        if (!camera_table) {
            return shared_ptr<SceneWorld>();
        }

        Vector3d camera_eye = camera_table->get_vector("center");
        if (!camera_eye.size()) {
            std::cerr << "ERROR: Error parsing camera center" << std::endl;
            return shared_ptr<SceneWorld>();
        }

        Vector3d camera_lookat = camera_table->get_vector("target");
        if (!camera_lookat.size()) {
            std::cerr << "ERROR: Error parsing camera target" << std::endl;
            return shared_ptr<SceneWorld>();
        }

        double camera_roll = camera_table->get_value("roll", 0);

        shared_ptr<ConfigTable> light_table = get_single_table(world_config, "light");
        if (!light_table) {
            return shared_ptr<SceneWorld>();
        }

        Vector3d light_center = light_table->get_vector("center");
        if (!light_center.size()) {
            std::cerr << "ERROR: Error parsing light center" << std::endl;
            return shared_ptr<SceneWorld>();
        }

        world_ptr->add_camera(shared_ptr<Camera>(
                                 new Camera(camera_eye, camera_lookat, camera_roll)));

        world_ptr->add_light(shared_ptr<Light>(new Light(light_center)));

        return world_ptr;
    }

    shared_ptr<ConfigTable> get_single_table(shared_ptr<ConfigReader> config,
                                                  const std::string& table_name) const
    {
        shared_ptr<ConfigTableIterator> it = config->get_tables(table_name);
        if (it) {
            it->first();
        }

        if (!it || it->is_done()) {
            std::cerr << "ERROR: No " << table_name.c_str() << " found" << std::endl;
            return shared_ptr<ConfigTable>();
        }

        shared_ptr<ConfigTable> table = it->current();

        it->next();
        if (!it->is_done()) {
            std::cerr << "ERROR: Multiple " << table_name.c_str() << "s found" << std::endl;
            return shared_ptr<ConfigTable>();
        }

        return table;
    }

    void process_actor_array(ActorType actor_type,
                             shared_ptr<ConfigTableIterator> it,
                             std::vector<shared_ptr<ActorBase> >* actor_ptrs) const
    {
        if (it)
        {
            for (it->first(); !it->is_done(); it->next())
            {
                if (actor_type == ActorType_Plane)
                    create_plane(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType_Sphere)
                    create_sphere(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType_Cylinder)
                    create_cylinder(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType_Triangle)
                    create_triangle(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType_Cube)
                    create_cube(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType_Molecule)
                    create_molecule(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType_Banner)
                    create_banner(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType_Mesh)
                    create_mesh(texture_factory_, it->current(), actor_ptrs);

                // Ignore when unknown type
            }
        }
    }

private:
    std::string world_filename_;
    TextureFactory* texture_factory_;
};


shared_ptr<SceneWorld> build_world(const std::string& world_filename,
                                        TextureFactory* texture_factory) {
    return WorldBuilder(
                world_filename,
                texture_factory
                ).build();
}


} //namespace mrtp
