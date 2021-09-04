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

class SceneWorld;
class WorldBuilder;

class ActorIterator;


class ActorIterator {
public:
    ActorIterator(std::vector<std::shared_ptr<ActorBase>>*);
    ActorIterator() = delete;
    ~ActorIterator() = default;

    void first();
    void next();
    bool is_done();
    std::vector<std::shared_ptr<ActorBase>>::iterator current();

private:
    std::vector<std::shared_ptr<ActorBase>>* actor_ptrs_;
    std::vector<std::shared_ptr<ActorBase>>::iterator actor_iter_;
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

    std::vector<std::shared_ptr<ActorBase>> actor_ptrs_;
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
