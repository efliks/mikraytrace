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

static const double kDegreeToRadian = M_PI / 180;
static const double kRealToByte = 255;

/*
distance: a distance to fully darken the light
shadow: darkness of shadows, between <0..1>
bias: correction to shadows to avoid self-intersection
distance: maximum distance reached by rays
maxdepth: number of recursion levels of a reflected ray
*/
Renderer::Renderer(SceneWorld *world, int width, int height, double fov,
                   double distance, double shadow, double bias, int maxdepth,
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

    ratio_ = static_cast<double>(width_) / static_cast<double>(height_);
    perspective_ = ratio_ / (2 * std::tan(kDegreeToRadian * fov_ / 2));

    Pixel dummy;
    framebuffer_.assign(width_ * height_, dummy);
}

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

bool Renderer::solve_shadows(const Eigen::Vector3d& origin,
                             const Eigen::Vector3d& direction,
                             double maxdist) const {
    ActorIterator actor_iterator = world_->get_actor_iterator();

    for (; !actor_iterator.is_done(); actor_iterator.next()) {
        ActorBase* actor = *actor_iterator.current();
        if (actor->has_shadow()) {
            double distance = actor->solve_light_ray(origin, direction, 0, maxdist);
            if (distance > 0) {
                return true;
            }
        }
    }
    return false;
}

ActorBase* Renderer::solve_hits(const Eigen::Vector3d& origin,
                            const Eigen::Vector3d& direction,
                            double* currd) const {
    ActorBase* hit = nullptr;

    ActorIterator actor_iterator = world_->get_actor_iterator();

    for (; !actor_iterator.is_done(); actor_iterator.next()) {
        ActorBase* actor = *actor_iterator.current();
        double distance = actor->solve_light_ray(origin, direction, 0, maxdist_);
        if (distance > 0 && distance < (*currd)) {
            *currd = distance;
            hit = actor;
        }
    }
    return hit;
}

Pixel Renderer::trace_ray_r(const Eigen::Vector3d& origin,
                            const Eigen::Vector3d& direction,
                            int depth) const {
    Pixel pixel;
    pixel << 0, 0, 0;

    double currd = maxdist_;
    ActorBase *hitactor = solve_hits(origin, direction, &currd);

    if (hitactor) {
        Eigen::Vector3d inter = (direction * currd) + origin;
        Eigen::Vector3d normal = hitactor->calculate_normal_at_hit(inter);

        // Calculate light intensity
        Light* my_light = world_->get_light_ptr();
        Eigen::Vector3d tolight = my_light->calculate_ray(inter);

        double lightd = tolight.norm();
        tolight *= (1 / lightd);

        double intensity = tolight.dot(normal);

        if (intensity > 0) {
            // Prevent self-intersection
            Eigen::Vector3d corr = inter + bias_ * normal;

            // Check if the intersection is in a shadow
            bool isshadow = solve_shadows(corr, tolight, lightd);
            double shadow = (isshadow) ? shadow_ : 1;

            // Decrease light intensity for actors away from the light
            double ambient = 1 - std::pow(lightd / maxdist_, 2);

            // Combine pixels
            double lambda = intensity * shadow * ambient;

            Pixel pick = hitactor->pick_pixel(inter, normal);
            pixel = (1 - lambda) * pixel + lambda * pick;

            // If the hit actor is reflective, trace a reflected ray
//            if (depth < maxdepth_) {
//                if (hitactor->reflect_coeff > 0) {
//                    Eigen::Vector3d ray = direction - (2 * direction.dot(normal)) * normal;
//                    Pixel reflected = trace_ray_r(corr, ray, depth + 1);
//                    pixel = (1 - hitactor->reflect_coeff) * reflected + hitactor->reflect_coeff * pixel;
//                }
//            }
        }
    }
    return pixel;
}

void Renderer::render_block(int block, int nlines) {
    Pixel *pixel = &framebuffer_[block * nlines * width_];

    Camera* my_camera = world_->get_camera_ptr();

    for (int j = 0; j < nlines; j++) {
        for (int i = 0; i < width_; i++, pixel++) {
            Eigen::Vector3d origin = my_camera->calculate_origin(i, j + block * nlines);
            Eigen::Vector3d direction = my_camera->calculate_direction(origin);
            *pixel = trace_ray_r(origin, direction, 0);
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
double Renderer::render_scene() {
    Camera* my_camera = world_->get_camera_ptr();

    my_camera->calculate_window(width_, height_, perspective_);

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
    double time_used = static_cast<double>(time_stop - time_start) / CLOCKS_PER_SEC;
    if (nthreads_ > 1) {
        time_used *= 1 / static_cast<double>(nthreads_);
    }
    return time_used;
}

} //namespace mrtp
