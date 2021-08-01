#include <fstream>
#include <iostream>
#include <Eigen/Geometry>
#include "world.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;


bool file_exists(const std::string& filename) {
    std::fstream check(filename.c_str());
    return check.good();
}


Vector3d fill_vector(const Vector3d& vec) {
    double x = (vec[0] < 0) ? -vec[0] : vec[0];
    double y = (vec[1] < 0) ? -vec[1] : vec[1];
    double z = (vec[2] < 0) ? -vec[2] : vec[2];

    Vector3d unit{0, 0, 1};

    if (x < y) {
        if (x < z) {
            unit << 1, 0, 0;
        }
    } else { // if ( x >= y)
        if (y < z) {
            unit << 0, 1, 0;
        }
    }
    return unit;
}


ActorFactory::ActorFactory(TextureFactory* texture_factory) :
    texture_factory_(texture_factory) {

}


PlaneActorFactory::PlaneActorFactory(TextureFactory* texture_factory,
                                     std::list<TexturedPlane>* planes) :
    ActorFactory(texture_factory),
    planes_(planes) {

}


SphereActorFactory::SphereActorFactory(TextureFactory* texture_factory,
                                       std::list<TexturedSphere>* spheres) :
    ActorFactory(texture_factory),
    spheres_(spheres) {

}


CylinderActorFactory::CylinderActorFactory(TextureFactory* texture_factory,
                                           std::list<TexturedCylinder>* cylinders) :
    ActorFactory(texture_factory),
    cylinders_(cylinders) {

}


ActorBase* PlaneActorFactory::create_actor(std::shared_ptr<cpptoml::table> plane_items) {
    auto plane_center = plane_items->get_array_of<double>("center");
    if (!plane_center) {
        //TODO
    }
    Vector3d plane_center_vec(plane_center->data());

    auto plane_normal = plane_items->get_array_of<double>("normal");
    if (!plane_normal) {
        //TODO
    }
    Vector3d plane_normal_vec(plane_normal->data());

    auto plane_texture = plane_items->get_as<std::string>("texture");
    if (!plane_texture) {
        //TODO
    }
    std::string plane_texture_str(plane_texture->data());
    if (!file_exists(plane_texture_str)) {
        //TODO
    }

    double scale_coef = plane_items->get_as<double>("scale").value_or(0.15);
    double reflect_coef = plane_items->get_as<double>("reflect").value_or(0);

    MyTexture* plane_texture_ptr = texture_factory_->create_texture(plane_texture_str, reflect_coef, scale_coef);

    Vector3d fill_vec = fill_vector(plane_normal_vec);

    Vector3d plane_vec_i = fill_vec.cross(plane_normal_vec);
    Vector3d plane_vec_j = plane_normal_vec.cross(plane_vec_i);

    plane_vec_i *= (1 / plane_vec_i.norm());
    plane_vec_j *= (1 / plane_vec_j.norm());
    plane_normal_vec *= (1 / plane_normal_vec.norm());

    StandardBasis plane_basis(
        plane_center_vec,
        plane_vec_i,
        plane_vec_j,
        plane_normal_vec
    );

    TexturedPlane new_plane(plane_basis, plane_texture_ptr);
    planes_->push_back(new_plane);

    return &planes_->back();
}


ActorBase* SphereActorFactory::create_actor(std::shared_ptr<cpptoml::table> sphere_items) {
    auto sphere_center = sphere_items->get_array_of<double>("center");
    if (!sphere_center) {
        //TODO
    }
    Vector3d sphere_center_vec(sphere_center->data());

    Vector3d sphere_axis_vec(0, 0, 1);
    auto sphere_axis = sphere_items->get_array_of<double>("axis");
    if (sphere_axis) {
        Vector3d tmp_vec(sphere_axis->data());
        sphere_axis_vec = tmp_vec;
    }

    auto sphere_texture = sphere_items->get_as<std::string>("texture");
    if (!sphere_texture) {
        //TODO
    }
    std::string sphere_texture_str(sphere_texture->data());
    if (!file_exists(sphere_texture_str)) {
        //TODO
    }

    double sphere_radius = sphere_items->get_as<double>("radius").value_or(1);
    double reflect_coef = sphere_items->get_as<double>("reflect").value_or(0);

    MyTexture* sphere_texture_ptr = texture_factory_->create_texture(sphere_texture_str, reflect_coef, 1);

    Vector3d fill_vec = fill_vector(sphere_axis_vec);

    Vector3d sphere_vec_i = fill_vec.cross(sphere_axis_vec);
    Vector3d sphere_vec_j = sphere_axis_vec.cross(sphere_vec_i);

    sphere_vec_i *= (1 / sphere_vec_i.norm());
    sphere_vec_j *= (1 / sphere_vec_j.norm());
    sphere_axis_vec *= (1 / sphere_axis_vec.norm());

    StandardBasis sphere_basis(
        sphere_center_vec,
        sphere_vec_i,
        sphere_vec_j,
        sphere_axis_vec
    );

    TexturedSphere new_sphere(
        sphere_basis,
        sphere_radius,
        sphere_texture_ptr
    );
    spheres_->push_back(new_sphere);

    return &spheres_->back();
}


ActorBase* CylinderActorFactory::create_actor(std::shared_ptr<cpptoml::table> cylinder_items) {
    auto cylinder_center = cylinder_items->get_array_of<double>("center");
    if (!cylinder_center) {
        //TODO
    }
    Vector3d cylinder_center_vec(cylinder_center->data());

    auto cylinder_direction = cylinder_items->get_array_of<double>("direction");
    if (!cylinder_direction) {
        //TODO
    }
    Vector3d cylinder_direction_vec(cylinder_direction->data());

    auto cylinder_texture = cylinder_items->get_as<std::string>("texture");
    if (!cylinder_texture) {
        //TODO
    }
    std::string cylinder_texture_str(cylinder_texture->data());
    if (!file_exists(cylinder_texture_str)) {
        //TODO
    }

    double cylinder_span = cylinder_items->get_as<double>("span").value_or(-1);
    double cylinder_radius = cylinder_items->get_as<double>("radius").value_or(1);
    double cylinder_reflect = cylinder_items->get_as<double>("reflect").value_or(0);

    MyTexture* cylinder_texture_ptr = texture_factory_->create_texture(cylinder_texture_str, cylinder_reflect, 1);

    Vector3d fill_vec = fill_vector(cylinder_direction_vec);

    Vector3d cylinder_vec_i = fill_vec.cross(cylinder_direction_vec);
    Vector3d cylinder_vec_j = cylinder_direction_vec.cross(cylinder_vec_i);

    cylinder_vec_i *= (1 / cylinder_vec_i.norm());
    cylinder_vec_j *= (1 / cylinder_vec_j.norm());
    cylinder_direction_vec *= (1 / cylinder_direction_vec.norm());

    StandardBasis cylinder_basis(
        cylinder_center_vec,
        cylinder_vec_i,
        cylinder_vec_j,
        cylinder_direction_vec
    );

    TexturedCylinder new_cylinder(
        cylinder_basis,
        cylinder_radius,
        cylinder_span,
        cylinder_texture_ptr
    );
    cylinders_->push_back(new_cylinder);

    return &cylinders_->back();
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


ActorIterator::ActorIterator(std::vector<ActorBase* >* actor_ptrs):
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

std::vector<ActorBase* >::iterator ActorIterator::current() {
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

    PlaneActorFactory plane_factory(texture_factory_, &my_world->planes_);
    SphereActorFactory sphere_factory(texture_factory_, &my_world->spheres_);
    CylinderActorFactory cylinder_factory(texture_factory_, &my_world->cylinders_);

    if (planes_array) {
        for (const auto& plane_items : *planes_array) {
            my_world->actor_ptrs_.push_back(plane_factory.create_actor(plane_items));
        }
    }

    if (spheres_array) {
        for (const auto& sphere_items : *spheres_array) {
            my_world->actor_ptrs_.push_back(sphere_factory.create_actor(sphere_items));
        }
    }

    if (cylinders_array) {
        for (const auto& cylinder_items : *cylinders_array) {
            my_world->actor_ptrs_.push_back(cylinder_factory.create_actor(cylinder_items));
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
