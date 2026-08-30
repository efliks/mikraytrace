#ifndef _RENDERER_H
#define _RENDERER_H

#include "vector3.h"
#include <vector>
#include <memory>

#include "actors.h"
#include "world.h"

namespace mrtp {

struct RendererConfig
{
    double fov;
    double ray_bias;
    double light_dist;
    double shadow_coeff;

    unsigned int width;
    unsigned int height;

    unsigned int max_recurse;
    unsigned int num_thread;

    const double fov_min;
    const double fov_max;

    const unsigned int width_min;
    const unsigned int width_max;

    const unsigned int height_min;
    const unsigned int height_max;

    const unsigned int num_min_thread;
    const unsigned int num_max_thread;
};


class SceneRendererBase {

public:
    SceneRendererBase(const RendererConfig&);
    virtual ~SceneRendererBase() {}

    virtual float do_render(SceneWorld*) = 0;

    //FIXME
    RendererConfig config_;
    std::vector<TexturePixel> framebuffer_;

protected:
    double ratio_;
    double perspective_;

    SceneWorld* scene_world_;

    Vector3d trace_ray_r(const Vector3d&, const Vector3d&, unsigned int) const;
    ActorBase* solve_hits(const Vector3d&, const Vector3d&, double*) const;
    bool solve_shadows(const Vector3d&, const Vector3d&, double) const;
    void render_block(unsigned int, unsigned int);
};


std::shared_ptr<SceneRendererBase> create_renderer(const RendererConfig&);

}

#endif  // _RENDERER_H
