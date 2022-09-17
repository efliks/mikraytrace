#include <Eigen/Core>
#include <easylogging++.h>

#include "config.h"
#include "world.h"

#include "actors/cube.h"
#include "actors/cylinder.h"
#include "actors/molecule.h"
#include "actors/plane.h"
#include "actors/sphere.h"
#include "actors/triangle.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;


void SceneWorld::add_light(std::shared_ptr<Light> light_ptr) {
    light_ = light_ptr;
}


void SceneWorld::add_camera(std::shared_ptr<Camera> camera_ptr) {
    camera_ = camera_ptr;
}


void SceneWorld::add_actor(std::shared_ptr<ActorBase> actor_ptr) {
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


ActorIterator::ActorIterator(std::vector<std::shared_ptr<ActorBase>>* actor_ptrs):
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


std::vector<std::shared_ptr<ActorBase>>::iterator ActorIterator::current() {
    return actor_iter_;
}


class WorldBuilder {
public:
    WorldBuilder(const std::string& world_filename,
                 TextureFactory* texture_factory) :
        world_filename_(world_filename),
        texture_factory_(texture_factory) {

    }

    WorldBuilder() = delete;
    ~WorldBuilder() = default;

    std::shared_ptr<SceneWorld> build() const
    {
        std::shared_ptr<BaseConfig> world_config = open_config(world_filename_);

        std::vector<std::shared_ptr<ActorBase>> new_actors;

        auto planes_array = world_config->get_tables("planes");
        process_actor_array(ActorType::Plane, planes_array, &new_actors);

        auto spheres_array = world_config->get_tables("spheres");
        process_actor_array(ActorType::Sphere, spheres_array, &new_actors);

        auto cylinders_array = world_config->get_tables("cylinders");
        process_actor_array(ActorType::Cylinder, cylinders_array, &new_actors);

        auto triangles_array = world_config->get_tables("triangles");
        process_actor_array(ActorType::Triangle, triangles_array, &new_actors);

        auto cubes_array = world_config->get_tables("cubes");
        process_actor_array(ActorType::Cube, cubes_array, &new_actors);

        auto molecules_array = world_config->get_tables("molecules");
        process_actor_array(ActorType::Molecule, molecules_array, &new_actors);

        if (new_actors.size() < 1) {
            LOG(ERROR) << "No actors found";
            return std::shared_ptr<SceneWorld>();
        }

        auto world_ptr = std::shared_ptr<SceneWorld>(new SceneWorld());
        for (const auto& actor : new_actors) {
            world_ptr->add_actor(actor);
        }

        std::shared_ptr<BaseTable> camera_table = world_config->get_table("camera");
        if (!camera_table) {
            LOG(ERROR) << "No camera found";
            return std::shared_ptr<SceneWorld>();
        }

        Vector3d camera_eye = camera_table->get_vector("center");
        if (!camera_eye.size()) {
            LOG(ERROR) << "Error parsing camera center";
            return std::shared_ptr<SceneWorld>();
        }

        Vector3d camera_lookat = camera_table->get_vector("target");
        if (!camera_lookat.size()) {
            LOG(ERROR) << "Error parsing camera target";
            return std::shared_ptr<SceneWorld>();
        }

        double camera_roll = camera_table->get_value("roll", 0);

        std::shared_ptr<BaseTable> light_table = world_config->get_table("light");
        if (!light_table) {
            LOG(ERROR) << "No light found";
            return std::shared_ptr<SceneWorld>();
        }

        Vector3d light_center = light_table->get_vector("center");
        if (!light_center.size()) {
            LOG(ERROR) << "Error parsing light center";
            return std::shared_ptr<SceneWorld>();
        }

        world_ptr->add_camera(std::shared_ptr<Camera>(
                                 new Camera(camera_eye, camera_lookat, camera_roll)));

        world_ptr->add_light(std::shared_ptr<Light>(new Light(light_center)));

        return world_ptr;
    }

    void process_actor_array(ActorType actor_type,
                             std::shared_ptr<BaseTableIterator> it,
                             std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) const
    {
        if (it)
        {
            for (it->first(); !it->is_done(); it->next())
            {
                if (actor_type == ActorType::Plane)
                    create_plane(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType::Sphere)
                    create_sphere(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType::Cylinder)
                    create_cylinder(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType::Triangle)
                    create_triangle(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType::Cube)
                    create_cube(texture_factory_, it->current(), actor_ptrs);
                else if (actor_type == ActorType::Molecule)
                    create_molecule(texture_factory_, it->current(), actor_ptrs);

                // Ignore when unknown type
            }
        }
    }

private:
    std::string world_filename_;
    TextureFactory* texture_factory_;
};


std::shared_ptr<SceneWorld> build_world(const std::string& world_filename,
                                        TextureFactory* texture_factory) {
    return WorldBuilder(
                world_filename,
                texture_factory
                ).build();
}


} //namespace mrtp
