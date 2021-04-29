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
    Renderer(World *world, int width, int height, double fov, double distance,
             double shadow, double bias, int maxdepth, int nthreads, 
             const char *path);
    ~Renderer();
    double render_scene();
    bool write_scene();

  private:
    World *world_;
    const char *path_;
    std::vector<Pixel> framebuffer_;
    int width_;
    int height_;
    int maxdepth_;
    int nthreads_;
    double maxdist_;
    double shadow_;
    double bias_;
    double fov_;
    double ratio_;
    double perspective_;

    bool solve_shadows(Eigen::Vector3d *origin, Eigen::Vector3d *direction,
                       double maxdist);
    Actor *solve_hits(Eigen::Vector3d *origin, Eigen::Vector3d *direction,
                      double *currd);
    Pixel trace_ray_r(Eigen::Vector3d *origin, Eigen::Vector3d *direction,
                      int depth);
    void render_block(int block, int nlines);
};

} //namespace mrtp

#endif //_RENDERER_H
