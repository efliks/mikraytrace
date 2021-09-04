#include <fstream>
#include <iostream>
#include <Eigen/Geometry>
#include "world.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;


static bool file_exists(const std::string& filename) {
    std::fstream check(filename.c_str());
    return check.good();
}


ActorIterator SceneWorld::get_actor_iterator() {
    return ActorIterator(&actor_ptrs_);
}


Camera* SceneWorld::get_camera_ptr() {
    return &(*cameras_.begin());  // ignore other cameras
}


Light* SceneWorld::get_light_ptr() {
    return &(*lights_.begin());  // ignore other lights
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


WorldBuilder::WorldBuilder(const std::string& world_filename,
                           TextureFactory* texture_factory) :
    world_filename_(world_filename),
    texture_factory_(texture_factory) {

}


Light WorldBuilder::make_light(std::shared_ptr<cpptoml::table> config) const {
    auto tab_light = config->get_table("light");
    if (!tab_light) {
        //TODO
    }

    auto raw_center = tab_light->get_array_of<double>("center");
    if (!raw_center) {
        //TODO
    }

    Vector3d temp_center(raw_center->data());
    Vector3d light_center = temp_center.cast<double>();

    return Light(light_center);
}


Camera WorldBuilder::make_camera(std::shared_ptr<cpptoml::table> config) const {
    auto tab_camera = config->get_table("camera");
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

    return Camera(camera_eye, camera_lookat, camera_roll);
}


std::shared_ptr<SceneWorld> WorldBuilder::build() const {
    if (!file_exists(world_filename_)) {
        //TODO
    }

    std::shared_ptr<cpptoml::table> world_config;
    try {
        world_config = cpptoml::parse_file(world_filename_.c_str());
    } catch (...) {
        //TODO
    }

    auto my_world = std::shared_ptr<SceneWorld>(new SceneWorld());

    auto planes_array = world_config->get_table_array("planes");
    auto spheres_array = world_config->get_table_array("spheres");
    auto cylinders_array = world_config->get_table_array("cylinders");

    TomlActorFactory actor_factory(texture_factory_);

    if (planes_array) {
        for (const auto& plane_items : *planes_array) {
            my_world->actor_ptrs_.push_back(actor_factory.create_plane(plane_items));
        }
    }

    if (spheres_array) {
        for (const auto& sphere_items : *spheres_array) {
            my_world->actor_ptrs_.push_back(actor_factory.create_sphere(sphere_items));
        }
    }

    if (cylinders_array) {
        for (const auto& cylinder_items : *cylinders_array) {
            my_world->actor_ptrs_.push_back(actor_factory.create_cylinder(cylinder_items));
        }
    }

    my_world->cameras_.push_back(make_camera(world_config));

    my_world->lights_.push_back(make_light(world_config));

    return my_world;
}


std::shared_ptr<SceneWorld> build_world(const std::string& world_filename,
                                        TextureFactory* texture_factory) {
    WorldBuilder my_builder(world_filename, texture_factory);
    std::shared_ptr<SceneWorld> my_world = my_builder.build();

    return my_world;
}


} //namespace mrtp
