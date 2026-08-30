#ifndef _WORLD_H
#define _WORLD_H

#include "shrptr.h"
#include <vector>

#include "actors.h"
#include "camera.h"
#include "light.h"
#include "texture.h"


namespace mrtp {

class ActorIterator {
public:
    ActorIterator(std::vector<shared_ptr<ActorBase> >*);

    void first();
    void next();
    bool is_done();
    std::vector<shared_ptr<ActorBase> >::iterator current();

private:
    std::vector<shared_ptr<ActorBase> >* actor_ptrs_;
    std::vector<shared_ptr<ActorBase> >::iterator actor_iter_;
};


class SceneWorld {
public:
    void add_light(shared_ptr<Light>);
    void add_camera(shared_ptr<Camera>);
    void add_actor(shared_ptr<ActorBase>);

    Light* get_light_ptr();
    Camera* get_camera_ptr();

    ActorIterator get_actor_iterator();

private:
    shared_ptr<Light> light_;
    shared_ptr<Camera> camera_;

    std::vector<shared_ptr<ActorBase> > actor_ptrs_;
};


shared_ptr<SceneWorld> build_world(const std::string&, TextureFactory*);


} //namespace mrtp

#endif //_WORLD_H
