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

SceneRendererBase::SceneRendererBase(SceneWorld* scene_world,
                                     const RendererConfig& config) :
    scene_world_(scene_world),
    config_(config) {

    ratio_ = static_cast<double>(config_.width) / static_cast<double>(config_.height);
    perspective_ = ratio_ / (2 * std::tan(M_PI / 180 * config_.field_of_vision / 2));

    framebuffer_.reserve(config_.width * config_.height);
}


bool SceneRendererBase::solve_shadows(const Vector3d& O,
                                      const Vector3d& D,
                                      double max_dist) const {
    ActorIterator actor_iterator = scene_world_->get_actor_iterator();

    for (; !actor_iterator.is_done(); actor_iterator.next()) {
        ActorBase* actor = *actor_iterator.current();
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
        ActorBase* actor = *actor_iterator.current();
        double distance = actor->solve_light_ray(O, D, 0, config_.max_distance);
        if (distance > 0 && distance < *curr_dist) {
            *curr_dist = distance;
            hit_actor = actor;
        }
    }
    return hit_actor;
}


Pixel SceneRendererBase::trace_ray_r(const Vector3d& O,
                                     const Vector3d& D,
                                     unsigned int depth) const {
    Pixel pixel{0, 0, 0};

    double curr_dist = config_.max_distance;
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
            double shadow = (is_shadow) ? config_.shadow_bias : 1;

            // Decrease light intensity for actors away from light
            double ambient = 1 - std::pow(light_dist / config_.max_distance, 2);

            // Combine pixels
            double lambda = intensity * shadow * ambient;

            // TODO Clean up!
            MyPixel my_pick = hit_actor->pick_pixel(inter, normal);
            Pixel pick(
                static_cast<double>(my_pick.pixel.red),
                static_cast<double>(my_pick.pixel.green),
                static_cast<double>(my_pick.pixel.blue)
            );
            pick /= 255;
            pixel = (1 - lambda) * pixel + lambda * pick;

            // If hit actor is reflective, trace reflected ray
            if (depth < config_.max_ray_depth) {
                if (my_pick.reflection_coeff > 0) {
                    Vector3d reflected_ray = D - (2 * D.dot(normal)) * normal;
                    Pixel reflected_pixel = trace_ray_r(inter_corr, reflected_ray, depth + 1);
                    pixel = (1 - my_pick.reflection_coeff) * reflected_pixel + my_pick.reflection_coeff * pixel;
                }
            }
        }
    }

    return pixel;
}


void SceneRendererBase::render_block(unsigned int block_index,
                                     unsigned int num_lines) {
    Camera* my_camera = scene_world_->get_camera_ptr();

    Pixel* pixel = &framebuffer_[block_index * num_lines * config_.width];

    for (unsigned int j = 0; j < num_lines; j++) {
        for (unsigned int i = 0; i < config_.width; i++, pixel++) {
            Vector3d origin = my_camera->calculate_origin(i, j + block_index * num_lines);
            Vector3d direction = my_camera->calculate_direction(origin);
            *pixel = trace_ray_r(origin, direction, 0);
        }
    }
}


ParallelSceneRenderer::ParallelSceneRenderer(SceneWorld* scene_world,
                                             const RendererConfig& render_config,
                                             unsigned int num_threads) :
    SceneRendererBase(scene_world, render_config),
    num_threads_(num_threads) {

}


void ParallelSceneRenderer::do_render() {
    Camera* my_camera = scene_world_->get_camera_ptr();

    my_camera->calculate_window(config_.width, config_.height, perspective_);

#ifdef _OPENMP
    if (num_threads_ == 1) {
        // Serial execution
        render_block(0, config_.height);
    } else {
        // Parallel execution
        if (num_threads_ != 0) {
            omp_set_num_threads(num_threads_);
        }
        unsigned int num_lines = config_.height / num_threads_;
        unsigned int block_index;

#pragma omp parallel for
        for (block_index = 0; block_index < num_threads_; block_index++) {
            render_block(block_index, num_lines);
        }

        unsigned int num_fill = config_.height % num_threads_;
        if (num_fill) {
            render_block(block_index + 1, num_fill);
        }
    }
#else
    // No OpenMP compiled in, always do serial execution
    render_block(0, config_.height);

#endif  // !_OPENMP
}


SceneRenderer::SceneRenderer(SceneWorld* scene_world,
                             const RendererConfig& render_config) :
    SceneRendererBase(scene_world, render_config) {

}


void SceneRenderer::do_render() {
    Camera* my_camera = scene_world_->get_camera_ptr();
    my_camera->calculate_window(config_.width, config_.height, perspective_);

    render_block(0, config_.height);
}


ScenePNGWriter::ScenePNGWriter(SceneRendererBase* scene_renderer) :
    scene_renderer_(scene_renderer) {

}


void ScenePNGWriter::write_to_file(const std::string& png_filename) {
    png::image<png::rgb_pixel> image(
                scene_renderer_->config_.width, scene_renderer_->config_.height);

    Pixel* in = &scene_renderer_->framebuffer_[0];

    for (unsigned int i = 0; i < scene_renderer_->config_.height; i++) {
        png::rgb_pixel* out = &image[i][0];
        for (unsigned int j = 0; j < scene_renderer_->config_.width; j++, in++, out++) {
            Pixel bytes = 255 * (*in);
            out->red = static_cast<unsigned char>(bytes[0]);
            out->green = static_cast<unsigned char>(bytes[1]);
            out->blue = static_cast<unsigned char>(bytes[2]);
        }
    }

    image.write(png_filename.c_str());
}


static const unsigned int kDefaultWidth = 640;
static const unsigned int kDefaultHeight = 480;
static const unsigned int kMinWidth = kDefaultWidth / 2;
static const unsigned int kMaxWidth = kDefaultWidth * 10;
static const unsigned int kMinHeight = kDefaultHeight / 2;
static const unsigned int kMaxHeight = kDefaultHeight * 10;

static const unsigned int kDefaultRecursionLevels = 3;
static const unsigned int kMinRecursionLevels = 0;
static const unsigned int kMaxRecursionLevels = 10;

static const unsigned int kDefaultThreads = 1;
static const unsigned int kMinThreads = 0;
static const unsigned int kMaxThreads = 64;

static const double kDefaultFOV = 93;
static const double kMinFOV = 50;
static const double kMaxFOV = 170;

static const double kDefaultDistance = 60;
static const double kDefaultShadow = 0.25;
static const double kDefaultBias = 0.001;


RendererConfig::RendererConfig() {
    field_of_vision = kDefaultFOV;
    max_distance = kDefaultDistance;
    shadow_bias = kDefaultShadow;
    ray_bias = kDefaultBias;

    width = kDefaultWidth;
    height = kDefaultHeight;

    max_ray_depth = kDefaultRecursionLevels;
    num_threads = kDefaultThreads;
}


bool RendererConfig::set_field_of_vision(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> field_of_vision;
    if (!convert) {
        return false;
    }
    if (field_of_vision < kMinFOV || field_of_vision > kMaxFOV) {
        return false;
    }
    return true;
}


bool RendererConfig::set_max_distance(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> max_distance;
    if (!convert) {
        return false;
    }
    return true;
}


bool RendererConfig::set_shadow_bias(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> shadow_bias;
    if (!convert) {
        return false;
    }
    return true;
}


bool RendererConfig::set_ray_bias(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> ray_bias;
    if (!convert) {
        return false;
    }
    return true;
}


bool RendererConfig::set_resolution(char* s) {
    std::string str(s);

    size_t p = str.find('x');
    if (p == std::string::npos) {
        p = str.find('X');
        if (p == std::string::npos) {
            // invalid format of resolution
            return false;
        }
    }
    std::string left(str.substr(0, p));
    std::stringstream convert(left);
    convert >> width;
    if (!convert) {
        // unable to convert width
        return false;
    }
    if (width < kMinWidth || width > kMaxWidth) {
        // width is out of range
        return false;
    }
    std::string right(str.substr(p+1, str.length()-p-1));
    std::stringstream convert_other(right);
    convert_other >> height;
    if (!convert_other) {
        // unable to convert heigth
        return false;
    }
    if (height < kMinHeight || height > kMaxHeight) {
        // height is out of range
        return false;
    }
    return true;
}


bool RendererConfig::set_max_ray_depth(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> max_ray_depth;
    if (!convert) {
        return false;
    }
    if (max_ray_depth < kMinRecursionLevels || max_ray_depth > kMaxRecursionLevels) {
        return false;
    }
    return true;
}


bool RendererConfig::set_num_threads(char* s) {
    std::string str(s);
    std::stringstream convert(str);

    convert >> num_threads;
    if (!convert) {
        return false;
    }
    if (num_threads < kMinThreads || num_threads > kMaxThreads) {
        return false;
    }
    return true;
}


}  //namespace mrtp
