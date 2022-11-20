#ifndef _RENDERER_H
#define _RENDERER_H

#include <Eigen/Core>
#include <vector>
#include <memory>

#include "actors.h"
#include "world.h"

using Vector3d = Eigen::Vector3d;


namespace mrtp {

struct RendererConfig
{
    double fov = 93;
    double ray_bias = 0.001;
    double light_dist = 60;
    double shadow_coeff = 0.25;

    unsigned int width = 640;
    unsigned int height = 480;

    unsigned int max_recurse = 3;
    unsigned int num_thread = 1;

    const double fov_min = 70;
    const double fov_max = 150;

    const unsigned int width_min = 320;
    const unsigned int width_max = 3200;

    const unsigned int height_min = 240;
    const unsigned int height_max = 2400;

    const unsigned int num_min_thread = 1;
    const unsigned int num_max_thread = 32;
};


class SceneRendererBase {

public:
    SceneRendererBase(const RendererConfig&);
    SceneRendererBase() = delete;
    virtual ~SceneRendererBase() = default;

    virtual float do_render(SceneWorld*) = 0;

    //FIXME
    RendererConfig config_;
    std::vector<TexturePixel> framebuffer_;

protected:
    double ratio_;
    double perspective_;

    SceneWorld* scene_world_;

    TexturePixel trace_ray_r(const Vector3d&, const Vector3d&, unsigned int) const;
    ActorBase* solve_hits(const Vector3d&, const Vector3d&, double*) const;
    bool solve_shadows(const Vector3d&, const Vector3d&, double) const;
    void render_block(unsigned int, unsigned int);
};


std::shared_ptr<SceneRendererBase> create_renderer(const RendererConfig&);

}

#endif  // _RENDERER_H
