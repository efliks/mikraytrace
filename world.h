#ifndef _WORLD_H
#define _WORLD_H

#include <memory>
#include <vector>

#include "actors.h"
#include "camera.h"
#include "light.h"
#include "texture.h"


namespace mrtp {

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
public:
    SceneWorld() = default;
    ~SceneWorld() = default;

    void add_light(std::shared_ptr<Light>);
    void add_camera(std::shared_ptr<Camera>);
    void add_actor(std::shared_ptr<ActorBase>);

    Light* get_light_ptr();
    Camera* get_camera_ptr();

    ActorIterator get_actor_iterator();

private:
    std::shared_ptr<Light> light_;
    std::shared_ptr<Camera> camera_;

    std::vector<std::shared_ptr<ActorBase>> actor_ptrs_;
};


std::shared_ptr<SceneWorld> build_world(const std::string&, TextureFactory*);


} //namespace mrtp

#endif //_WORLD_H
