#include <fstream>
#include <iostream>
#include <Eigen/Geometry>
#include "cpptoml.h"
#include "world.h"


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
            //TODO
        }

        std::shared_ptr<cpptoml::table> world_config;
        try {
            world_config = cpptoml::parse_file(world_filename_.c_str());
        } catch (...) {
            //TODO
        }

        auto world_ptr = std::shared_ptr<SceneWorld>(new SceneWorld());

        auto planes_array = world_config->get_table_array("planes");
        auto spheres_array = world_config->get_table_array("spheres");
        auto cylinders_array = world_config->get_table_array("cylinders");
        auto triangles_array = world_config->get_table_array("triangles");

        if (planes_array) {
            for (const auto& plane_items : *planes_array) {
                world_ptr->add_actor(create_actor(ActorType::Plane, texture_factory_, plane_items));
            }
        }
        if (spheres_array) {
            for (const auto& sphere_items : *spheres_array) {
                world_ptr->add_actor(create_actor(ActorType::Sphere, texture_factory_, sphere_items));
            }
        }
        if (cylinders_array) {
            for (const auto& cylinder_items : *cylinders_array) {
                world_ptr->add_actor(create_actor(ActorType::Cylinder, texture_factory_, cylinder_items));
            }
        }
        if (triangles_array) {
            for (const auto& triangle_items : *triangles_array) {
                world_ptr->add_actor(create_actor(ActorType::Triangle, texture_factory_, triangle_items));
            }
        }

        auto tab_camera = world_config->get_table("camera");
        if (!tab_camera) {
            //TODO
        }
        auto raw_eye = tab_camera->get_array_of<double>("center");
        if (!raw_eye) {
            //TODO
        }
        auto raw_lookat = tab_camera->get_array_of<double>("target");
        if (!raw_lookat) {
            //TODO
        }

        double camera_roll = tab_camera->get_as<double>("roll").value_or(0);

        Vector3d temp_eye(raw_eye->data());
        Vector3d camera_eye = temp_eye.cast<double>();
        Vector3d temp_lookat(raw_lookat->data());
        Vector3d camera_lookat = temp_lookat.cast<double>();

        auto tab_light = world_config->get_table("light");
        if (!tab_light) {
            //TODO
        }
        auto raw_center = tab_light->get_array_of<double>("center");
        if (!raw_center) {
            //TODO
        }

        Vector3d temp_center(raw_center->data());
        Vector3d light_center = temp_center.cast<double>();

        world_ptr->add_camera(std::shared_ptr<Camera>(
                                 new Camera(camera_eye, camera_lookat, camera_roll)));

        world_ptr->add_light(std::shared_ptr<Light>(new Light(light_center)));

        return world_ptr;
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
