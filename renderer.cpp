#include <Eigen/Geometry>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "png.hpp"
#include "renderer.h"

#ifdef _OPENMP
#include <omp.h>
#endif


namespace mrtp {

static const float kDegreeToRadian = M_PI / 180.0f;
static const float kRealToByte = 255.0f;

/*
distance: a distance to fully darken the light
shadow: darkness of shadows, between <0..1>
bias: correction to shadows to avoid self-intersection
distance: maximum distance reached by rays
maxdepth: number of recursion levels of a reflected ray
*/
Renderer::Renderer(World *world, int width, int height, float fov,
                   float distance, float shadow, float bias, int maxdepth,
                   int nthreads, const char *path) : 
    world_(world), 
    width_(width), 
    height_(height), 
    fov_(fov), 
    maxdist_(distance), 
    shadow_(shadow), 
    bias_(bias), 
    maxdepth_(maxdepth), 
    nthreads_(nthreads), 
    path_(path) {

    ratio_ = static_cast<float>(width_) / static_cast<float>(height_);
    perspective_ = ratio_ / (2.0f * std::tan(kDegreeToRadian * fov_ / 2.0f));

    Pixel dummy;
    framebuffer_.assign(width_ * height_, dummy);
}

Renderer::~Renderer() {}

bool Renderer::write_scene() {
    png::image<png::rgb_pixel> image(width_, height_);
    Pixel *in = &framebuffer_[0];

    for (int i = 0; i < height_; i++) {
        png::rgb_pixel *out = &image[i][0];

        for (int j = 0; j < width_; j++, in++, out++) {
            Pixel bytes = kRealToByte * (*in);
            out->red = static_cast<unsigned char>(bytes[0]);
            out->green = static_cast<unsigned char>(bytes[1]);
            out->blue = static_cast<unsigned char>(bytes[2]);
        }
    }
    image.write(path_);
    return rs_ok;
}

bool Renderer::solve_shadows(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                             float maxdist) {
    std::vector<Actor *> actors = world_->ptr_actors_;
    std::vector<Actor *>::iterator iter = actors.begin();
    std::vector<Actor *>::iterator iter_end = actors.end();

    for (; iter != iter_end; ++iter) {
        Actor *actor = *iter;
        if (actor->has_shadow()) {
            float distance = actor->solve(origin, direction, 0.0f, maxdist);
            if (distance > 0.0f) {
                return true;
            }
        }
    }
    return false;
}

Actor *Renderer::solve_hits(Eigen::Vector3f *origin, Eigen::Vector3f *direction,
                            float *currd) {
    std::vector<Actor *> actors = world_->ptr_actors_;
    std::vector<Actor *>::iterator iter = actors.begin();
    std::vector<Actor *>::iterator iter_end = actors.end();
    Actor *hit = nullptr;

    for (; iter != iter_end; ++iter) {
        Actor *actor = *iter;
        float distance = actor->solve(origin, direction, 0.0f, maxdist_);
        if ((distance > 0.0f) && (distance < (*currd))) {
            *currd = distance;
            hit = actor;
        }
    }
    return hit;
}

Pixel Renderer::trace_ray_r(Eigen::Vector3f *origin, Eigen::Vector3f *direction, int depth) {
    Pixel pixel;
    pixel << 0.0f, 0.0f, 0.0f;

    float currd = maxdist_;
    Actor *hitactor = solve_hits(origin, direction, &currd);

    if (hitactor) {
        Eigen::Vector3f inter = ((*direction) * currd) + (*origin);
        Eigen::Vector3f normal = hitactor->calculate_normal(&inter);

        // Calculate light intensity
        Eigen::Vector3f tolight = world_->ptr_light_->calculate_ray(&inter);

        float lightd = tolight.norm();
        tolight *= (1.0f / lightd);

        float intensity = tolight.dot(normal);

        if (intensity > 0.0f) {
            // Prevent self-intersection
            Eigen::Vector3f corr = inter + bias_ * normal;

            // Check if the intersection is in a shadow
            bool isshadow = solve_shadows(&corr, &tolight, lightd);
            float shadow = (isshadow) ? shadow_ : 1.0f;

            // Decrease light intensity for actors away from the light
            float ambient = 1.0f - std::pow(lightd / maxdist_, 2);

            // Combine pixels
            float lambda = intensity * shadow * ambient;

            Pixel pick = hitactor->pick_pixel(&inter, &normal);
            pixel = (1.0f - lambda) * pixel + lambda * pick;

            // If the hit actor is reflective, trace a reflected ray
            if (depth < maxdepth_) {
                float coeff = hitactor->get_reflect();
                if (coeff > 0.0f) {
                    Eigen::Vector3f ray = (*direction) - (2.0f * direction->dot(normal)) * normal;
                    Pixel reflected = trace_ray_r(&corr, &ray, depth + 1);
                    pixel = (1.0f - coeff) * reflected + coeff * pixel;
                }
            }
        }
    }
    return pixel;
}

void Renderer::render_block(int block, int nlines) {
    Pixel *pixel = &framebuffer_[block * nlines * width_];

    for (int j = 0; j < nlines; j++) {
        for (int i = 0; i < width_; i++, pixel++) {
            Eigen::Vector3f origin = world_->ptr_camera_->calculate_origin(i, j + block * nlines);
            Eigen::Vector3f direction = world_->ptr_camera_->calculate_direction(&origin);
            *pixel = trace_ray_r(&origin, &direction, 0);
        }
    }
}

/*
In parallel mode, splits the frame buffer into several
horizontal blocks, each rendered by a separate thread.

After each thread has finished, there may still be
some left-over lines to render.

If nthreads=0, uses as many threads as available.

Returns rendering time in seconds, corrected for
the number of threads.
*/
float Renderer::render_scene() {
    world_->ptr_camera_->calculate_window(width_, height_, perspective_);

    int time_start = clock();

#ifdef _OPENMP
    if (nthreads_ == 1) {
        // Serial execution
        render_block(0, height_);
    } else {
        // Parallel execution
        if (nthreads_ != 0) {
            omp_set_num_threads(nthreads_);
        }
        int nlines = height_ / nthreads_;
        int block;

#pragma omp parallel for
        for (block = 0; block < nthreads_; block++) {
            render_block(block, nlines);
        }

        int nfill = height_ % nthreads_;
        if (nfill) {
            render_block(block + 1, nfill);
        }
    }
#else
    // No OpenMP compiled in, always do serial execution
    render_block(0, height_);

#endif //!_OPENMP

    int time_stop = std::clock();
    float time_used = static_cast<float>(time_stop - time_start) / CLOCKS_PER_SEC;
    if (nthreads_ > 1) {
        time_used *= 1.0f / static_cast<float>(nthreads_);
    }
    return time_used;
}

} //namespace mrtp
