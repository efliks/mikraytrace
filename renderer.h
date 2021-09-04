#ifndef _RENDERER_H
#define _RENDERER_H

#include <Eigen/Core>
#include <vector>

#include "actors.h"
#include "camera.h"
#include "light.h"
#include "pixel.h"
#include "world.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;

class ScenePNGWriter;


class RendererConfig {
public:
    RendererConfig();
    ~RendererConfig() = default;

    double field_of_vision;
    double max_distance;
    double shadow_bias;
    double ray_bias;

    unsigned int buffer_width;
    unsigned int buffer_height;

    unsigned int max_ray_depth;
    unsigned int num_threads;
};


class SceneRendererBase {
    friend class ScenePNGWriter;

public:
    SceneRendererBase(SceneWorld*, const RendererConfig&);
    SceneRendererBase() = delete;
    virtual ~SceneRendererBase() = default;

    virtual void do_render() = 0;

protected:
    double ratio_;
    double perspective_;

    SceneWorld* scene_world_;
    RendererConfig config_;
    std::vector<Pixel> framebuffer_;

    Pixel trace_ray_r(const Vector3d&, const Vector3d&, unsigned int) const;
    ActorBase* solve_hits(const Vector3d&, const Vector3d&, double*) const;
    bool solve_shadows(const Vector3d&, const Vector3d&, double) const;
    void render_block(unsigned int, unsigned int);
};


class ParallelSceneRenderer : public SceneRendererBase {
public:
    ParallelSceneRenderer(SceneWorld*, const RendererConfig&, unsigned int);
    ParallelSceneRenderer() = delete;
    ~ParallelSceneRenderer() override = default;

    void do_render() override;

private:
    unsigned int num_threads_;
};


class SceneRenderer : public SceneRendererBase {
public:
    SceneRenderer(SceneWorld*, const RendererConfig&);
    SceneRenderer() = delete;
    ~SceneRenderer() override = default;

    void do_render() override;
};


class ScenePNGWriter {
public:
    ScenePNGWriter(SceneRendererBase*);
    ScenePNGWriter() = delete;
    ~ScenePNGWriter() = default;

    void write_to_file(const std::string&);

private:
    SceneRendererBase* scene_renderer_;
};


}  //namespace mrtp

#endif  //_RENDERER_H
