#include <Eigen/Geometry>

#include <cstdlib>
#include <cmath>
#include <ctime>

#include "renderer.h"
#include "camera.h"
#include "light.h"
#include "logger.h"

#ifdef _OPENMP
#include <omp.h>
#endif

constexpr double pi() { return std::atan(1) * 4; }


namespace mrtp {

SceneRendererBase::SceneRendererBase(const RendererConfig& config)
    : config_(config)
{
    ratio_ = static_cast<double>(config_.width) / static_cast<double>(config_.height);
    perspective_ = ratio_ / (2 * std::tan(pi() / 180 * config_.fov / 2));

    framebuffer_.reserve(config_.width * config_.height);
}


bool SceneRendererBase::solve_shadows(const Vector3d& O,
                                      const Vector3d& D,
                                      double max_dist) const {
    ActorIterator actor_iterator = scene_world_->get_actor_iterator();

    for (; !actor_iterator.is_done(); actor_iterator.next()) {
        std::shared_ptr<ActorBase> actor = *actor_iterator.current();
        if (actor->has_shadow()) {
            double distance = actor->solve_light_ray(O, D, 0, max_dist);
            if (distance > 0) {
                return true;
            }
        }
    }
    return false;
}


ActorBase* SceneRendererBase::solve_hits(const Vector3d& O,
                                         const Vector3d& D,
                                         double* curr_dist) const {
    ActorBase* hit_actor = nullptr;

    ActorIterator actor_iterator = scene_world_->get_actor_iterator();

    for (; !actor_iterator.is_done(); actor_iterator.next()) {
        std::shared_ptr<ActorBase> actor = *actor_iterator.current();
        double distance = actor->solve_light_ray(O, D, 0, config_.light_dist);
        if (distance > 0 && distance < *curr_dist) {
            *curr_dist = distance;
            hit_actor = actor.get();
        }
    }
    return hit_actor;
}


TexturePixel SceneRendererBase::trace_ray_r(const Vector3d& O,
        const Vector3d& D, unsigned int depth) const
{
    Vector3d pixel_vec{0, 0, 0};

    double curr_dist = config_.light_dist;
    ActorBase* hit_actor = solve_hits(O, D, &curr_dist);

    if (hit_actor) {
        Light* my_light = scene_world_->get_light_ptr();

        Vector3d inter = (D * curr_dist) + O;
        Vector3d normal = hit_actor->calculate_normal_at_hit(inter);
        Vector3d to_light = my_light->calculate_ray(inter);

        // Calculate light intensity
        double light_dist = to_light.norm();
        to_light *= (1 / light_dist);

        double intensity = to_light.dot(normal);

        if (intensity > 0) {
            // Prevent self-intersection
            Vector3d inter_corr = inter + config_.ray_bias * normal;

            // Check if intersection is in shadow
            bool is_shadow = solve_shadows(inter_corr, to_light, light_dist);
            double shadow = (is_shadow) ? config_.shadow_coeff : 1;

            // Decrease light intensity for actors away from light
            double ambient = 1 - std::pow(light_dist / config_.light_dist, 2);

            // Combine pixels
            double lambda = intensity * shadow * ambient;

            MyPixel my_pick = hit_actor->pick_pixel(inter, normal);
            Vector3d pick = my_pick.pixel.to_vec();
            pixel_vec = (1 - lambda) * pixel_vec + lambda * pick;

            // If hit actor is reflective, trace reflected ray
            if (depth < config_.max_recurse) {
                if (my_pick.reflection_coeff > 0) {
                    Vector3d reflected_ray = D - (2 * D.dot(normal)) * normal;
                    TexturePixel reflected_pixel = trace_ray_r(inter_corr, reflected_ray, depth + 1);

                    Vector3d reflected_vec = reflected_pixel.to_vec();
                    Vector3d combined_vec = (1 - my_pick.reflection_coeff) * reflected_vec + my_pick.reflection_coeff * pixel_vec;

                    pixel_vec = combined_vec;
                }
            }
        }
    }

    return TexturePixel(pixel_vec);
}


void SceneRendererBase::render_block(unsigned int block_index,
                                     unsigned int num_lines) {
    Camera* my_camera = scene_world_->get_camera_ptr();

    TexturePixel* pixel = &framebuffer_[block_index * num_lines * config_.width];

    for (unsigned int j = 0; j < num_lines; j++) {
        for (unsigned int i = 0; i < config_.width; i++, pixel++) {
            Vector3d origin = my_camera->calculate_origin(i, j + block_index * num_lines);
            Vector3d direction = my_camera->calculate_direction(origin);
            *pixel = trace_ray_r(origin, direction, 0);
        }
    }
}


ParallelSceneRenderer::ParallelSceneRenderer(const RendererConfig& render_config)
    : SceneRendererBase(render_config)
{
}


float ParallelSceneRenderer::do_render(SceneWorld* scene_world)
{
    scene_world_ = scene_world;
    Camera* my_camera = scene_world_->get_camera_ptr();

    my_camera->calculate_window(config_.width, config_.height, perspective_);
    
    long time_start = clock();

#ifdef _OPENMP
    if (num_threads_ == 1) {
        // Serial execution
        render_block(0, config_.buffer_height);
    } else {
        // Parallel execution
        if (num_threads_ != 0) {
            omp_set_num_threads(num_threads_);
        }
        unsigned int num_lines = config_.buffer_height / num_threads_;
        unsigned int block_index;

#pragma omp parallel for
        for (block_index = 0; block_index < num_threads_; block_index++) {
            render_block(block_index, num_lines);
        }

        unsigned int num_fill = config_.buffer_height % num_threads_;
        if (num_fill) {
            render_block(block_index + 1, num_fill);
        }
    }
#else
    // No OpenMP compiled in, always do serial execution
    render_block(0, config_.height);

#endif  // !_OPENMP

    long time_elapsed = std::clock() - time_start;
    float time_used = static_cast<float>(time_elapsed) / CLOCKS_PER_SEC / config_.num_thread;

    return time_used;
}


SceneRenderer::SceneRenderer(const RendererConfig& render_config)
    : SceneRendererBase(render_config)
{
}


float SceneRenderer::do_render(SceneWorld* scene_world)
{
    scene_world_ = scene_world;
    Camera* my_camera = scene_world_->get_camera_ptr();
    my_camera->calculate_window(config_.width, config_.height, perspective_);

    long time_start = clock();

    render_block(0, config_.height);

    return static_cast<float>(std::clock() - time_start) / CLOCKS_PER_SEC;
}


std::shared_ptr<SceneRendererBase> create_renderer(const RendererConfig& config)
{
    if (config.num_thread > 1) {
        return std::shared_ptr<SceneRendererBase>(new ParallelSceneRenderer(config));
    }

    return std::shared_ptr<SceneRendererBase>(new SceneRenderer(config));
}


}  //namespace mrtp
