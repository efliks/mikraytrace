#include <fstream>
#include <iostream>
#include "world.h"


namespace mrtp {

//Local functions

static bool file_exists(const char *path) {
    std::fstream check(path);
    return check.good();
}

static bool read_vector(std::shared_ptr<cpptoml::table> items, const char *id, 
                        Eigen::Vector3d *vector) {
    auto raw = items->get_array_of<double>(id);
    if (!raw) { return false; }
    Eigen::Vector3d tmp(raw->data());
    *vector = tmp.cast<double>();
    return true;
}

static bool read_texture(std::shared_ptr<cpptoml::table> items, std::string *output) {
    auto raw = items->get_as<std::string>("texture");
    if (!raw) { return false; }
    const char *texture = raw->data();
    if (!file_exists(texture)) { return false; }
    *output = texture;
    return true;
}

//Member functions

World::World(const std::string& world_filename, TextureCollector* texture_collector) :
    world_filename_(world_filename), texture_collector_(texture_collector) {

}


WorldStatus_t World::initialize() {
    if (!file_exists(world_filename_.c_str())) {
        return ws_no_file;
    }

    std::shared_ptr<cpptoml::table> config;
    try {
      config = cpptoml::parse_file(world_filename_.c_str());
    } catch (...) {
      return ws_parse_error;
    }

    auto tab_camera = config->get_table("camera");
    if (!tab_camera) { return ws_no_camera; }
    
    auto raw_eye = tab_camera->get_array_of<double>("center");
    if (!raw_eye) { return ws_camera_param; }
    
    auto raw_lookat = tab_camera->get_array_of<double>("target");
    if (!raw_lookat) { return ws_camera_param; }

    double camera_roll = static_cast<double>(tab_camera->get_as<double>("roll").value_or(0));

    Eigen::Vector3d temp_eye(raw_eye->data());
    Eigen::Vector3d camera_eye = temp_eye.cast<double>();
    Eigen::Vector3d temp_lookat(raw_lookat->data());
    Eigen::Vector3d camera_lookat = temp_lookat.cast<double>();

    Camera camera = Camera(camera_eye, camera_lookat, camera_roll);
    cameras_.push_back(camera);
    ptr_camera_ = &cameras_.back();

    auto tab_light = config->get_table("light");
    if (!tab_light) { return ws_no_light; }

    auto raw_center = tab_light->get_array_of<double>("center");
    if (!raw_center) { return ws_light_param; }

    Eigen::Vector3d temp_center(raw_center->data());
    Eigen::Vector3d light_center = temp_center.cast<double>();

    Light light = Light(light_center);
    lights_.push_back(light);
    ptr_light_ = &lights_.back();

    WorldStatus_t check;

    auto planes = config->get_table_array("planes");
    if ((check = load_planes(planes)) != ws_ok) { return check; }

    auto spheres = config->get_table_array("spheres");
    if ((check = load_spheres(spheres)) != ws_ok) { return check; }

    auto cylinders = config->get_table_array("cylinders");
    if ((check = load_cylinders(cylinders)) != ws_ok) { return check; }

    if (ptr_actors_.empty()) { return ws_no_actors; }

    return ws_ok;
}

WorldStatus_t World::load_planes(std::shared_ptr<cpptoml::table_array> array) {
    if (!array) { return ws_ok; }
    for (const auto& items : *array) {
        WorldStatus_t check;
        if ((check = load_plane(items)) != ws_ok) { return check; }
    }
    return ws_ok;
}

WorldStatus_t World::load_spheres(std::shared_ptr<cpptoml::table_array> array) {
    if (!array) { return ws_ok; }
    for (const auto& items : *array) {
        WorldStatus_t check;
        if ((check = load_sphere(items)) != ws_ok) { return check; }
    }
    return ws_ok;
}

WorldStatus_t World::load_cylinders(std::shared_ptr<cpptoml::table_array> array) {
    if (!array) { return ws_ok; }
    for (const auto& items : *array) {
        WorldStatus_t check;
        if ((check = load_cylinder(items)) != ws_ok) { return check; }
    }
    return ws_ok;
}

WorldStatus_t World::load_plane(std::shared_ptr<cpptoml::table> items) {
    Eigen::Vector3d center;
    if (!read_vector(items, "center", &center)) { return ws_plane_param; }

    Eigen::Vector3d normal;
    if (!read_vector(items, "normal", &normal)) { return ws_plane_param; }

    std::string texture;
    if (!read_texture(items, &texture)) { return ws_plane_texture; }

    double scale = static_cast<double>(items->get_as<double>("scale").value_or(0.15));
    double reflect = static_cast<double>(items->get_as<double>("reflect").value_or(0));

    Plane plane(center, normal, scale, reflect, texture, texture_collector_);
    planes_.push_back(plane);
    ptr_actors_.push_back(&planes_.back());

    return ws_ok;
}

WorldStatus_t World::load_sphere(std::shared_ptr<cpptoml::table> items) {
    Eigen::Vector3d center;
    if (!read_vector(items, "center", &center)) { return ws_sphere_param; }

    Eigen::Vector3d axis(0, 0, 1);
    read_vector(items, "axis", &axis);

    std::string texture;
    if (!read_texture(items, &texture)) { return ws_sphere_texture; }

    double radius = static_cast<double>(items->get_as<double>("radius").value_or(1));
    double reflect = static_cast<double>(items->get_as<double>("reflect").value_or(0));

    Sphere sphere(center, axis, radius, reflect, texture, texture_collector_);
    spheres_.push_back(sphere);
    ptr_actors_.push_back(&spheres_.back());

    return ws_ok;
}

WorldStatus_t World::load_cylinder(std::shared_ptr<cpptoml::table> items) {
    Eigen::Vector3d center;
    if (!read_vector(items, "center", &center)) { return ws_cylinder_param; }

    Eigen::Vector3d direction;
    if (!read_vector(items, "direction", &direction)) { return ws_cylinder_param; }

    std::string texture;
    if (!read_texture(items, &texture)) { return ws_cylinder_texture; }

    double span = static_cast<double>(items->get_as<double>("span").value_or(-1));
    double radius = static_cast<double>(items->get_as<double>("radius").value_or(1));
    double reflect = static_cast<double>(items->get_as<double>("reflect").value_or(0));

    Cylinder cylinder(center, direction, radius, span, reflect, texture, texture_collector_);
    cylinders_.push_back(cylinder);
    ptr_actors_.push_back(&cylinders_.back());

    return ws_ok;
}


SceneWorld::SceneWorld(TextureCollector* texture_collector) :
    texture_collector_(texture_collector) {

}

void SceneWorld::add_light(const Light& light) {
    lights_.push_back(light);
}

void SceneWorld::add_camera(const Camera& camera) {
    cameras_.push_back(camera);
}

void SceneWorld::add_plane(const Plane& plane) {
    planes_.push_back(plane);
    actor_ptrs_.push_back(&planes_.back());
}

void SceneWorld::add_sphere(const Sphere& sphere) {
    spheres_.push_back(sphere);
    actor_ptrs_.push_back(&spheres_.back());
}

void SceneWorld::add_cylinder(const Cylinder& cylinder) {
    cylinders_.push_back(cylinder);
    actor_ptrs_.push_back(&cylinders_.back());
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


ActorIterator::ActorIterator(std::vector<Actor* >* actor_ptrs):
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

std::vector<Actor* >::iterator ActorIterator::current() {
    return actor_iter_;
}


WorldBuilder::WorldBuilder(const std::string& world_filename,
                           TextureCollector* texture_collector) :
    world_filename_(world_filename),
    texture_collector_(texture_collector) {

}


Plane WorldBuilder::make_plane(std::shared_ptr<cpptoml::table> plane_items) const {
    auto plane_center = plane_items->get_array_of<double>("center");
    if (!plane_center) {
        //TODO
    }
    Eigen::Vector3d plane_center_vec(plane_center->data());

    auto plane_normal = plane_items->get_array_of<double>("normal");
    if (!plane_normal) {
        //TODO
    }
    Eigen::Vector3d plane_normal_vec(plane_normal->data());

    auto plane_texture = plane_items->get_as<std::string>("texture");
    if (!plane_texture) {
        //TODO
    }
    std::string plane_texture_str(plane_texture->data());
    if (!file_exists(plane_texture_str.c_str())) {
        //TODO
    }

    double scale_coef = plane_items->get_as<double>("scale").value_or(0.15);
    double reflect_coef = plane_items->get_as<double>("reflect").value_or(0);

    Plane new_plane(
        plane_center_vec,
        plane_normal_vec,
        scale_coef,
        reflect_coef,
        plane_texture_str,
        texture_collector_
    );
    return new_plane;
}


Sphere WorldBuilder::make_sphere(std::shared_ptr<cpptoml::table> sphere_items) const {
    auto sphere_center = sphere_items->get_array_of<double>("center");
    if (!sphere_center) {
        //TODO
    }
    Eigen::Vector3d sphere_center_vec(sphere_center->data());

    Eigen::Vector3d sphere_axis_vec(0, 0, 1);
    auto sphere_axis = sphere_items->get_array_of<double>("axis");
    if (sphere_axis) {
        Eigen::Vector3d tmp_vec(sphere_axis->data());
        sphere_axis_vec = tmp_vec;
    }

    auto sphere_texture = sphere_items->get_as<std::string>("texture");
    if (!sphere_texture) {
        //TODO
    }
    std::string sphere_texture_str(sphere_texture->data());
    if (!file_exists(sphere_texture_str.c_str())) {
        //TODO
    }

    double sphere_radius = sphere_items->get_as<double>("radius").value_or(1);
    double reflect_coef = sphere_items->get_as<double>("reflect").value_or(0);

    Sphere new_sphere(
        sphere_center_vec,
        sphere_axis_vec,
        sphere_radius,
        reflect_coef,
        sphere_texture_str,
        texture_collector_
    );
    return new_sphere;
}


Cylinder WorldBuilder::make_cylinder(std::shared_ptr<cpptoml::table> cylinder_items) const {
    auto cylinder_center = cylinder_items->get_array_of<double>("center");
    if (!cylinder_center) {
        //TODO
    }
    Eigen::Vector3d cylinder_center_vec(cylinder_center->data());

    auto cylinder_direction = cylinder_items->get_array_of<double>("direction");
    if (!cylinder_direction) {
        //TODO
    }
    Eigen::Vector3d cylinder_direction_vec(cylinder_direction->data());

    auto cylinder_texture = cylinder_items->get_as<std::string>("texture");
    if (!cylinder_texture) {
        //TODO
    }
    std::string cylinder_texture_str(cylinder_texture->data());
    if (!file_exists(cylinder_texture_str.c_str())) {
        //TODO
    }

    double cylinder_span = cylinder_items->get_as<double>("span").value_or(-1);
    double cylinder_radius = cylinder_items->get_as<double>("radius").value_or(1);
    double cylinder_reflect = cylinder_items->get_as<double>("reflect").value_or(0);

    Cylinder new_cylinder(
        cylinder_center_vec,
        cylinder_direction_vec,
        cylinder_radius,
        cylinder_span,
        cylinder_reflect,
        cylinder_texture_str,
        texture_collector_
    );
    return new_cylinder;
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

    Eigen::Vector3d temp_center(raw_center->data());
    Eigen::Vector3d light_center = temp_center.cast<double>();

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

    Eigen::Vector3d temp_eye(raw_eye->data());
    Eigen::Vector3d camera_eye = temp_eye.cast<double>();
    Eigen::Vector3d temp_lookat(raw_lookat->data());
    Eigen::Vector3d camera_lookat = temp_lookat.cast<double>();

    return Camera(camera_eye, camera_lookat, camera_roll);
}


std::shared_ptr<SceneWorld> WorldBuilder::build() const {
    if (!file_exists(world_filename_.c_str())) {
        //TODO
    }

    std::shared_ptr<cpptoml::table> world_config;
    try {
        world_config = cpptoml::parse_file(world_filename_.c_str());
    } catch (...) {
        //TODO
    }

    auto my_world = std::shared_ptr<SceneWorld>(new SceneWorld(texture_collector_));

    auto planes_array = world_config->get_table_array("planes");
    auto spheres_array = world_config->get_table_array("spheres");
    auto cylinders_array = world_config->get_table_array("cylinders");

    if (planes_array) {
        for (const auto& plane_items : *planes_array) {
            my_world->add_plane(make_plane(plane_items));
        }
    }

    if (spheres_array) {
        for (const auto& sphere_items : *spheres_array) {
            my_world->add_sphere(make_sphere(sphere_items));
        }
    }

    if (cylinders_array) {
        for (const auto& cylinder_items : *cylinders_array) {
            my_world->add_cylinder(make_cylinder(cylinder_items));
        }
    }

    my_world->add_camera(make_camera(world_config));

    my_world->add_light(make_light(world_config));

    return my_world;
}


std::shared_ptr<SceneWorld> build_world(const std::string& world_filename,
                                        TextureCollector* texture_collector) {
    WorldBuilder my_builder(world_filename, texture_collector);
    std::shared_ptr<SceneWorld> my_world = my_builder.build();

    return my_world;
}


} //namespace mrtp
