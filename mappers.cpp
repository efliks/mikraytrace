#include "mappers.h"


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
        double fracy = phi / M_PI;

        double dot_vi = normal_at_hit.dot(local_basis.vi);
        double theta = std::acos(dot_vi / std::sin(phi)) / (2 * M_PI);

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
        double frac_x = acos(dot) / M_PI;
        double frac_y = alpha / (2 * M_PI * radius_);

        return texture_->pick_pixel(frac_x, frac_y);
    }

private:
    MyTexture* texture_;
    double radius_;
};


std::shared_ptr<TextureMapper> create_texture_mapper(std::shared_ptr<cpptoml::table> actor_items,
                                                     ActorType actor_type,
                                                     TextureFactory* texture_factory) {
    double reflect_coef = actor_items->get_as<double>("reflect").value_or(0);
    auto actor_texture = actor_items->get_as<std::string>("texture");

    // TODO Do texture mapping for triangles - currently only one color
    if (actor_texture && actor_type != ActorType::Triangle) {
        auto actor_color = actor_items->get_array_of<double>("color");
        if (actor_color) {
            //Both color and texture are set
        }

        std::string texture_str(actor_texture->data());
        std::fstream check(texture_str.c_str());
        if (!check.good()) {
            //Texture file cannot be opened
        }

        double scale_coef = actor_items->get_as<double>("scale").value_or(0.15);
        if (actor_type == ActorType::Sphere) {
            scale_coef = actor_items->get_as<double>("scale").value_or(1);
        }

        MyTexture* texture_ptr = texture_factory->create_texture(
                                    texture_str, reflect_coef, scale_coef);

        if (actor_type == ActorType::Plane) {
            return std::shared_ptr<TextureMapper>(new PlaneTextureMapper(texture_ptr));
        }
        else if (actor_type == ActorType::Sphere) {
            return std::shared_ptr<TextureMapper>(new SphereTextureMapper(texture_ptr));
        }
        else {
            double cylinder_radius = actor_items->get_as<double>("radius").value_or(1);
            return std::shared_ptr<TextureMapper>(new CylinderTextureMapper(texture_ptr, cylinder_radius));
        }
    }

    auto actor_color = actor_items->get_array_of<double>("color");
    if (actor_color) {
        Vector3d color_vec(actor_color->data());
        TexturePixel color(color_vec);
        return std::shared_ptr<TextureMapper>(new DummyTextureMapper(color, reflect_coef));
    }

    //Neither texture nor color are set
}


std::shared_ptr<TextureMapper> create_dummy_mapper(std::shared_ptr<cpptoml::table> items,
                                                   const std::string& color_str,
                                                   const std::string& reflect_str) {
    auto actor_color = items->get_array_of<double>(color_str);

    if (actor_color) {
        double reflect_coef = items->get_as<double>(reflect_str).value_or(0);
        Vector3d color_vec(actor_color->data());
        TexturePixel color(color_vec);

        return std::shared_ptr<TextureMapper>(new DummyTextureMapper(color, reflect_coef));
    }

    //TODO No color set
}


}
