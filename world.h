#ifndef _WORLD_H
#define _WORLD_H

#include <memory>
#include <vector>
#include <list>
#include "cpptoml.h"

#include "actors.h"
#include "camera.h"
#include "light.h"
#include "texture.h"


namespace mrtp {

enum WorldStatus_t {ws_ok, ws_no_file, ws_parse_error, ws_no_camera, 
                    ws_no_light, ws_no_actors, ws_camera_param, 
                    ws_light_param, ws_plane_param, ws_sphere_param, 
                    ws_cylinder_param, ws_plane_texture, ws_sphere_texture, 
                    ws_cylinder_texture};


class World {
  public:
    World(const std::string& world_filename,
          TextureCollector* texture_collector);
    World() = delete;
    ~World() = default;

    WorldStatus_t initialize();

    Camera *ptr_camera_;
    Light *ptr_light_;
    std::vector<Actor *> ptr_actors_;

  private:
    WorldStatus_t load_plane(std::shared_ptr<cpptoml::table> items);
    WorldStatus_t load_sphere(std::shared_ptr<cpptoml::table> items);
    WorldStatus_t load_cylinder(std::shared_ptr<cpptoml::table> items);
    
    WorldStatus_t load_planes(std::shared_ptr<cpptoml::table_array> array);
    WorldStatus_t load_spheres(std::shared_ptr<cpptoml::table_array> array);
    WorldStatus_t load_cylinders(std::shared_ptr<cpptoml::table_array> array);

    std::list<Camera> cameras_;
    std::list<Light> lights_;
    std::list<Plane> planes_;
    std::list<Sphere> spheres_;
    std::list<Cylinder> cylinders_;

    std::string world_filename_;
    TextureCollector* texture_collector_;
};


class ActorIterator {
public:
    ActorIterator(std::vector<Actor* >* actor_ptrs);
    ActorIterator() = delete;
    ~ActorIterator() = default;

    void first();
    void next();
    bool is_done();
    std::vector<Actor* >::iterator current();

private:
    std::vector<Actor* >* actor_ptrs_;
    std::vector<Actor* >::iterator actor_iter_;
};


class SceneWorld {
public:
    SceneWorld(TextureCollector* texture_collector);
    SceneWorld() = delete;
    ~SceneWorld() = default;

    void add_light(const Light& light);
    void add_camera(const Camera& camera);
    void add_plane(const Plane& plane);
    void add_sphere(const Sphere& sphere);
    void add_cylinder(const Cylinder& cylinder);

    Light get_light();
    Camera get_camera();

    ActorIterator get_actor_iterator();

private:
    std::list<Light> lights_;
    std::list<Camera> cameras_;
    std::list<Plane> planes_;
    std::list<Sphere> spheres_;
    std::list<Cylinder> cylinders_;

    std::vector<Actor* > actor_ptrs_;

    TextureCollector* texture_collector_;
};


class WorldBuilder {
public:
    WorldBuilder(const std::string& world_filename,
                 TextureCollector* texture_collector);
    WorldBuilder() = delete;
    ~WorldBuilder() = default;

    Light make_light(std::shared_ptr<cpptoml::table> light_items) const;
    Camera make_camera(std::shared_ptr<cpptoml::table> camera_items) const;

    Plane make_plane(std::shared_ptr<cpptoml::table> plane_items) const;
    Sphere make_sphere(std::shared_ptr<cpptoml::table> sphere_items) const;
    Cylinder make_cylinder(std::shared_ptr<cpptoml::table> cylinder_items) const;

    SceneWorld build() const;

private:
    std::string world_filename_;
    TextureCollector* texture_collector_;
};


SceneWorld build_world(const std::string& world_filename,
                       TextureCollector* texture_collector);


} //namespace mrtp

#endif //_WORLD_H
