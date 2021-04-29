#include <fstream>
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

World::World(const char *path) : path_(path) {}

World::~World() {}

WorldStatus_t World::initialize() {
    if (!file_exists(path_)) { return ws_no_file; }

    std::shared_ptr<cpptoml::table> config;
    try { config = cpptoml::parse_file(path_); } catch (...) { return ws_parse_error; }

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

    Camera camera(&camera_eye, &camera_lookat, camera_roll);
    cameras_.push_back(camera);
    ptr_camera_ = &cameras_.back();

    auto tab_light = config->get_table("light");
    if (!tab_light) { return ws_no_light; }

    auto raw_center = tab_light->get_array_of<double>("center");
    if (!raw_center) { return ws_light_param; }

    Eigen::Vector3d temp_center(raw_center->data());
    Eigen::Vector3d light_center = temp_center.cast<double>();

    Light light(&light_center);
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

    Plane plane(&center, &normal, scale, reflect, texture.c_str());
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

    Sphere sphere(&center, &axis, radius, reflect, texture.c_str());
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

    Cylinder cylinder(&center, &direction, radius, span, reflect, texture.c_str());
    cylinders_.push_back(cylinder);
    ptr_actors_.push_back(&cylinders_.back());

    return ws_ok;
}

} //namespace mrtp
