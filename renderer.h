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

enum RendererStatus_t {rs_ok, rs_fail};


class Renderer {
  public:
    Renderer(World *world, int width, int height, float fov, float distance,
             float shadow, float bias, int maxdepth, int nthreads, 
             const char *path);
    ~Renderer();
    float render_scene();
    bool write_scene();

  private:
    World *world_;
    const char *path_;
    std::vector<Pixel> framebuffer_;
    int width_;
    int height_;
    int maxdepth_;
    int nthreads_;
    float maxdist_;
    float shadow_;
    float bias_;
    float fov_;
    float ratio_;
    float perspective_;

    bool solve_shadows(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                       float maxdist);
    Actor *solve_hits(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                      float *currd);
    Pixel trace_ray_r(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                      int depth);
    void render_block(int block, int nlines);
};

} //namespace mrtp

#endif //_RENDERER_H
