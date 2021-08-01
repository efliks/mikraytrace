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


class PlaneActorFactory;
class SphereActorFactory;
class CylinderActorFactory;

class SceneWorld;
class WorldBuilder;

class ActorIterator;


class ActorFactory {
public:
    ActorFactory(TextureFactory*);
    ActorFactory() = delete;
    virtual ~ActorFactory() = default;

    virtual ActorBase* create_actor(std::shared_ptr<cpptoml::table>) = 0;

protected:
    TextureFactory* texture_factory_;
};


class PlaneActorFactory : public ActorFactory {
public:
    PlaneActorFactory(TextureFactory*, std::list<TexturedPlane>*);
    PlaneActorFactory() = delete;
    ~PlaneActorFactory() override = default;

    ActorBase* create_actor(std::shared_ptr<cpptoml::table>) override;

private:
    std::list<TexturedPlane>* planes_;
};


class SphereActorFactory : public ActorFactory {
public:
    SphereActorFactory(TextureFactory*, std::list<TexturedSphere>*);
    SphereActorFactory() = delete;
    ~SphereActorFactory() override = default;

    ActorBase* create_actor(std::shared_ptr<cpptoml::table>) override;

private:
    std::list<TexturedSphere>* spheres_;
};


class CylinderActorFactory : public ActorFactory {
public:
    CylinderActorFactory(TextureFactory*, std::list<TexturedCylinder>*);
    CylinderActorFactory() = delete;
    ~CylinderActorFactory() override = default;

    ActorBase* create_actor(std::shared_ptr<cpptoml::table>) override;

private:
    std::list<TexturedCylinder>* cylinders_;
};


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
    friend class WorldBuilder;

public:
    SceneWorld() = default;
    ~SceneWorld() = default;

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
};


class WorldBuilder {
public:
    WorldBuilder(const std::string&, TextureFactory*);
    WorldBuilder() = delete;
    ~WorldBuilder() = default;

    std::shared_ptr<SceneWorld> build() const;

    Light make_light(std::shared_ptr<cpptoml::table>) const;
    Camera make_camera(std::shared_ptr<cpptoml::table>) const;

private:
    std::string world_filename_;

    TextureFactory* texture_factory_;
};


std::shared_ptr<SceneWorld> build_world(const std::string& world_filename,
                                        TextureFactory* texture_factory);


} //namespace mrtp

#endif //_WORLD_H
