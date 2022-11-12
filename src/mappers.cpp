#include <fstream>
#include <cmath>

#include "logger.h"
#include "mappers.h"

constexpr double pi() { return std::atan(1) * 4; }


namespace mrtp {

class DummyTextureMapper : public TextureMapper {
public:
    DummyTextureMapper(TexturePixel color, double reflection_coef) :
        color_(color),
        reflection_coef_(reflection_coef) {
    }

    ~DummyTextureMapper() override = default;

    MyPixel pick_pixel(const Vector3d& hit,
                       const Vector3d& normal_at_hit,
                       const StandardBasis& local_basis) const override {
        return MyPixel{color_, reflection_coef_};
    }

private:
    TexturePixel color_;
    double reflection_coef_;
};


class PlaneTextureMapper : public TextureMapper {
public:
    PlaneTextureMapper(MyTexture* texture) :
        texture_(texture) {
    }

    ~PlaneTextureMapper() override = default;

    MyPixel pick_pixel(const Vector3d& hit,
                       const Vector3d& normal_at_hit,
                       const StandardBasis& local_basis) const override {
        Vector3d v = hit - local_basis.o;
        double tx_i = v.dot(local_basis.vi);
        double tx_j = v.dot(local_basis.vj);

        return texture_->pick_pixel(tx_i, tx_j);
    }

private:
    MyTexture* texture_;
};


class SphereTextureMapper : public TextureMapper {
public:
    SphereTextureMapper(MyTexture* texture) :
        texture_(texture) {
    }

    ~SphereTextureMapper() override = default;

    MyPixel pick_pixel(const Vector3d& hit,
                       const Vector3d& normal_at_hit,
                       const StandardBasis& local_basis) const override {
        // Taken from https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html
        double dot_vj = normal_at_hit.dot(local_basis.vj);
        double phi = std::acos(-dot_vj);
        double fracy = phi / pi();

        double dot_vi = normal_at_hit.dot(local_basis.vi);
        double theta = std::acos(dot_vi / std::sin(phi)) / (2 * pi());

        double dot_vk = normal_at_hit.dot(local_basis.vk);
        double fracx = (dot_vk > 0) ? theta : (1 - theta);

        return texture_->pick_pixel(fracx, fracy);
    }

private:
    MyTexture* texture_;
};


class CylinderTextureMapper : public TextureMapper {
public:
    CylinderTextureMapper(MyTexture* texture, double radius) :
        texture_(texture),
        radius_(radius) {
    }

    ~CylinderTextureMapper() override = default;

    MyPixel pick_pixel(const Vector3d& hit,
                       const Vector3d& normal_at_hit,
                       const StandardBasis& local_basis) const override {
        Vector3d t = hit - local_basis.o;

        double alpha = t.dot(local_basis.vk);
        double dot = normal_at_hit.dot(local_basis.vi);
        double frac_x = acos(dot) / pi();
        double frac_y = alpha / (2 * pi() * radius_);

        return texture_->pick_pixel(frac_x, frac_y);
    }

private:
    MyTexture* texture_;
    double radius_;
};


std::shared_ptr<TextureMapper> create_texture_mapper(std::shared_ptr<ConfigTable> actor_items,
                                                     ActorType actor_type,
                                                     TextureFactory* texture_factory)
{
    double reflect_coef = actor_items->get_value("reflect", 0);
    std::string actor_texture = actor_items->get_text("texture");

    if (!actor_texture.empty()) {

        Vector3d actor_color = actor_items->get_vector("color");
        if (!actor_color.size()) {
            LOG_WARNING("Ignoring color and using texture file");
        }

        std::fstream check(actor_texture);
        if (!check.good()) {
            LOG_ERROR(std::string("Cannot open texture file " + actor_texture));
            return std::shared_ptr<TextureMapper>();
        }

        double default_coef = (actor_type == ActorType::Sphere) ? 1 : 0.15;
        double scale_coef = actor_items->get_value("scale", default_coef);

        MyTexture* texture_ptr = texture_factory->create_texture(
                                    actor_texture, reflect_coef, scale_coef);

        if (actor_type == ActorType::Plane) {
            return std::shared_ptr<TextureMapper>(new PlaneTextureMapper(texture_ptr));
        }
        else if (actor_type == ActorType::Sphere) {
            return std::shared_ptr<TextureMapper>(new SphereTextureMapper(texture_ptr));
        }
        else if (actor_type == ActorType::Cylinder) {
            double cylinder_radius = actor_items->get_value("radius", 1);
            return std::shared_ptr<TextureMapper>(new CylinderTextureMapper(texture_ptr, cylinder_radius));
        }

        // Unknown actor type
        return std::shared_ptr<TextureMapper>();
    }

    Vector3d actor_color = actor_items->get_vector("color");
    if (actor_color.size()) {
        TexturePixel pixel_color(actor_color);
        return std::shared_ptr<TextureMapper>(new DummyTextureMapper(pixel_color, reflect_coef));
    }

    LOG_ERROR("Cannot parse texture file and color for texture mapper");
    return std::shared_ptr<TextureMapper>();
}


std::shared_ptr<TextureMapper> create_dummy_mapper(std::shared_ptr<ConfigTable> items,
                                                   const std::string& color_str,
                                                   const std::string& reflect_str)
{
    Vector3d actor_color = items->get_vector(color_str);

    if (actor_color.size()) {
        double reflect_coef = items->get_value(reflect_str, 0);
        TexturePixel pixel_color(actor_color);

        return std::shared_ptr<TextureMapper>(new DummyTextureMapper(pixel_color, reflect_coef));
    }

    LOG_ERROR("Color for texture mapper not found");
    return std::shared_ptr<TextureMapper>();
}


}
