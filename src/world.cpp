#include <fstream>
#include <iostream>
#include <Eigen/Geometry>
#include <easylogging++.h>

#include "cpptoml.h"
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

    std::shared_ptr<SceneWorld> build() const {
        std::fstream check(world_filename_.c_str());
        if (!check.good()) {
            LOG(ERROR) << "Cannot open world file";
            return std::shared_ptr<SceneWorld>();
        }

        std::shared_ptr<cpptoml::table> world_config;
        try {
            world_config = cpptoml::parse_file(world_filename_.c_str());
        } catch (...) {
            LOG(ERROR) << "Error parsing world file";
            return std::shared_ptr<SceneWorld>();
        }

        std::vector<std::shared_ptr<ActorBase>> new_actors;

        auto planes_array = world_config->get_table_array("planes");
        process_actor_array(ActorType::Plane, planes_array, &new_actors);

        auto spheres_array = world_config->get_table_array("spheres");
        process_actor_array(ActorType::Sphere, spheres_array, &new_actors);

        auto cylinders_array = world_config->get_table_array("cylinders");
        process_actor_array(ActorType::Cylinder, cylinders_array, &new_actors);

        auto triangles_array = world_config->get_table_array("triangles");
        process_actor_array(ActorType::Triangle, triangles_array, &new_actors);

        auto cubes_array = world_config->get_table_array("cubes");
        process_actor_array(ActorType::Cube, cubes_array, &new_actors);

        auto molecules_array = world_config->get_table_array("molecules");
        process_actor_array(ActorType::Molecule, molecules_array, &new_actors);

        if (new_actors.size() < 1) {
            LOG(ERROR) << "No actors found";
            return std::shared_ptr<SceneWorld>();
        }

        auto world_ptr = std::shared_ptr<SceneWorld>(new SceneWorld());
        for (const auto& actor : new_actors) {
            world_ptr->add_actor(actor);
        }

        auto tab_camera = world_config->get_table("camera");
        if (!tab_camera) {
            LOG(ERROR) << "No camera found";
            return std::shared_ptr<SceneWorld>();
        }
        auto raw_eye = tab_camera->get_array_of<double>("center");
        if (!raw_eye) {
            LOG(ERROR) << "Error parsing camera center";
            return std::shared_ptr<SceneWorld>();
        }
        auto raw_lookat = tab_camera->get_array_of<double>("target");
        if (!raw_lookat) {
            LOG(ERROR) << "Error parsing camera target";
            return std::shared_ptr<SceneWorld>();
        }

        double camera_roll = tab_camera->get_as<double>("roll").value_or(0);

        Vector3d temp_eye(raw_eye->data());
        Vector3d camera_eye = temp_eye.cast<double>();
        Vector3d temp_lookat(raw_lookat->data());
        Vector3d camera_lookat = temp_lookat.cast<double>();

        auto tab_light = world_config->get_table("light");
        if (!tab_light) {
            LOG(ERROR) << "No light found";
            return std::shared_ptr<SceneWorld>();
        }
        auto raw_center = tab_light->get_array_of<double>("center");
        if (!raw_center) {
            LOG(ERROR) << "Error parsing light center";
            return std::shared_ptr<SceneWorld>();
        }

        Vector3d temp_center(raw_center->data());
        Vector3d light_center = temp_center.cast<double>();

        world_ptr->add_camera(std::shared_ptr<Camera>(
                                 new Camera(camera_eye, camera_lookat, camera_roll)));

        world_ptr->add_light(std::shared_ptr<Light>(new Light(light_center)));

        return world_ptr;
    }

    void process_actor_array(ActorType actor_type,
                             std::shared_ptr<cpptoml::table_array> actor_array,
                             std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) const
    {
        if (actor_array) {
            for (const auto& actor_items : *actor_array) {
                if (actor_type == ActorType::Plane)
                    create_plane(texture_factory_, actor_items, actor_ptrs);
                else if (actor_type == ActorType::Sphere)
                    create_sphere(texture_factory_, actor_items, actor_ptrs);
                else if (actor_type == ActorType::Cylinder)
                    create_cylinder(texture_factory_, actor_items, actor_ptrs);
                else if (actor_type == ActorType::Triangle)
                    create_triangle(texture_factory_, actor_items, actor_ptrs);
                else if (actor_type == ActorType::Cube)
                    create_cube(texture_factory_, actor_items, actor_ptrs);
                else if (actor_type == ActorType::Molecule)
                    create_molecule(texture_factory_, actor_items, actor_ptrs);

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
