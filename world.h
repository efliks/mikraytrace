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


class ActorIterator {
public:
    ActorIterator(std::vector<ActorBase* >* actor_ptrs);
    ActorIterator() = delete;
    ~ActorIterator() = default;

    void first();
    void next();
    bool is_done();
    std::vector<ActorBase* >::iterator current();

private:
    std::vector<ActorBase* >* actor_ptrs_;
    std::vector<ActorBase* >::iterator actor_iter_;
};


class SceneWorld {
public:
    SceneWorld(TextureCollector* texture_collector);
    SceneWorld() = delete;
    ~SceneWorld() = default;

    void add_light(const Light& light);
    void add_camera(const Camera& camera);
    void add_plane(const TexturedPlane& plane);
    void add_sphere(const TexturedSphere& sphere);
    void add_cylinder(const TexturedCylinder& cylinder);

    Light* get_light_ptr();
    Camera* get_camera_ptr();

    ActorIterator get_actor_iterator();

private:
    std::list<Light> lights_;
    std::list<Camera> cameras_;
    std::list<TexturedPlane> planes_;
    std::list<TexturedSphere> spheres_;
    std::list<TexturedCylinder> cylinders_;

    std::vector<ActorBase* > actor_ptrs_;

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

    TexturedPlane make_plane(std::shared_ptr<cpptoml::table> plane_items) const;
    TexturedSphere make_sphere(std::shared_ptr<cpptoml::table> sphere_items) const;
    TexturedCylinder make_cylinder(std::shared_ptr<cpptoml::table> cylinder_items) const;

    std::shared_ptr<SceneWorld> build() const;

private:
    std::string world_filename_;
    TextureCollector* texture_collector_;
};


std::shared_ptr<SceneWorld> build_world(const std::string& world_filename,
                                        TextureCollector* texture_collector);


} //namespace mrtp

#endif //_WORLD_H
