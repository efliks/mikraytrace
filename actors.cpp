#include <Eigen/Geometry>
#include <cmath>
#include "actors.h"


namespace mrtp {

using Vector3d = Eigen::Vector3d;

const double kMyZero = 0.0001;


static double solve_quadratic(double a, double b, double c) {
    double delta = b * b - 4 * a * c;
    if (delta < 0) {
        return -1;
    }

    if (delta < kMyZero && -delta > -kMyZero) {
        return -b / (2 * a);
    }

    double sqdelta = sqrt(delta);
    double t = 0.5 / a;
    double ta = (-b - sqdelta) * t;
    double tb = (-b + sqdelta) * t;

    return (ta < tb) ? ta : tb;
}


static Vector3d fill_vector(const Vector3d& vec) {
    double x = (vec[0] < 0) ? -vec[0] : vec[0];
    double y = (vec[1] < 0) ? -vec[1] : vec[1];
    double z = (vec[2] < 0) ? -vec[2] : vec[2];

    Vector3d unit{0, 0, 1};

    if (x < y) {
        if (x < z) {
            unit << 1, 0, 0;
        }
    } else { // if ( x >= y)
        if (y < z) {
            unit << 0, 1, 0;
        }
    }
    return unit;
}


ActorBase::ActorBase(const StandardBasis& local_basis,
                     std::shared_ptr<TextureMapper> texture_mapper_ptr) :
    local_basis_(local_basis),
    texture_mapper_(texture_mapper_ptr) {

}


MyPixel ActorBase::pick_pixel(const Vector3d& X, const Vector3d& N) const {
    return texture_mapper_->pick_pixel(X, N, local_basis_);
}


class SimplePlane : public ActorBase {
public:
    SimplePlane(const StandardBasis& local_basis,
                std::shared_ptr<TextureMapper> texture_mapper_ptr) :
        ActorBase(local_basis, texture_mapper_ptr) {
    }

    ~SimplePlane() override = default;

    bool has_shadow() const override {
        return false;
    }

    double solve_light_ray(const Vector3d& O, const Vector3d& D,
                           double min_dist, double max_dist) const override {
        double t = D.dot(local_basis_.vk);
        if (t > kMyZero || t < -kMyZero) {
            Vector3d v = O - local_basis_.o;
            double d = -v.dot(local_basis_.vk) / t;
            if (d > min_dist && d < max_dist) {
                return d;
            }
        }
        return -1;
    }

    Vector3d calculate_normal_at_hit(const Vector3d& hit) const override {
        return local_basis_.vk;
    }
};


class SimpleTriangle : public ActorBase {
public:
    SimpleTriangle(const StandardBasis& local_basis,
                   const Vector3d& A, const Vector3d& B, const Vector3d& C,
                   std::shared_ptr<TextureMapper> texture_mapper_ptr) :
        ActorBase(local_basis, texture_mapper_ptr),
        A_(A), B_(B), C_(C) {

        TA_ = local_basis_.vk.cross(A - C);
        TB_ = local_basis_.vk.cross(B - A);
        TC_ = local_basis_.vk.cross(C - B);
    }

    ~SimpleTriangle() override = default;

    bool has_shadow() const override {
        return true;
    }

    double solve_light_ray(const Vector3d& O, const Vector3d& D,
                           double min_dist, double max_dist) const override {
        SimplePlane plane(local_basis_, texture_mapper_);
        double t = plane.solve_light_ray(O, D, min_dist, max_dist);
        if (t > 0) {
            Vector3d X = O + t * D;
            if ((X - A_).dot(TA_) > 0 &&
                (X - B_).dot(TB_) > 0 &&
                (X - C_).dot(TC_) > 0) {
                return t;
            }
        }
        return -1;
    }

    Vector3d calculate_normal_at_hit(const Vector3d& hit) const override {
        return local_basis_.vk;
    }

private:
    Vector3d A_;
    Vector3d B_;
    Vector3d C_;
    Vector3d TA_;
    Vector3d TB_;
    Vector3d TC_;
};


class SimpleSphere : public ActorBase {
public:
    SimpleSphere(const StandardBasis& local_basis,
                 double radius,
                 std::shared_ptr<TextureMapper> texture_mapper_ptr) :
        ActorBase(local_basis, texture_mapper_ptr),
        radius_(radius) {
    }

    ~SimpleSphere() override = default;

    bool has_shadow() const override {
        return true;
    }

    double solve_light_ray(const Vector3d& O, const Vector3d& D,
                           double min_dist, double max_dist) const override {
        Vector3d t = O - local_basis_.o;

        double a = D.dot(D);
        double b = 2 * D.dot(t);
        double c = t.dot(t) - radius_ * radius_;
        double d = solve_quadratic(a, b, c);

        if (d > min_dist && d < max_dist) {
            return d;
        }
        return -1;
    }

    Vector3d calculate_normal_at_hit(const Vector3d& hit) const override {
        Vector3d t = hit - local_basis_.o;
        return t * (1 / t.norm());
    }

private:
    double radius_;
};


class SimpleCylinder : public ActorBase {
public:
    SimpleCylinder(const StandardBasis& local_basis,
                   double radius, double length,
                   std::shared_ptr<TextureMapper> texture_mapper_ptr) :
        ActorBase(local_basis, texture_mapper_ptr),
        radius_(radius),
        length_(length) {
    }

    ~SimpleCylinder() override = default;

    bool has_shadow() const override {
        return true;
    }

    /*
    Capital letters are vectors.
      A       Origin    of cylinder
      B       Direction of cylinder
      O       Origin    of ray
      D       Direction of ray
      P       Hit point on cylinder's surface
      X       Point on cylinder's axis closest to the hit point
      t       Distance between ray's      origin and P
      alpha   Distance between cylinder's origin and X

     (P - X) . B = 0
     |P - X| = R  => (P - X) . (P - X) = R^2

     P = O + t * D
     X = A + alpha * B
     T = O - A
     ...
     2t * (T.D - alpha * D.B)  +  t^2 - 2 * alpha * T.B  +
         +  alpha^2  =  R^2 - T.T
     a = T.D
     b = D.B
     d = T.B
     f = R^2 - T.T

     t^2 * (1 - b^2)  +  2t * (a - b * d)  -
         -  d^2 - f = 0    => t = ...
     alpha = d + t * b
    */
    double solve_light_ray(const Vector3d& O, const Vector3d& D,
                           double min_dist, double max_dist) const override {
        Vector3d vec = O - local_basis_.o;

        double a = D.dot(vec);
        double b = D.dot(local_basis_.vk);
        double d = vec.dot(local_basis_.vk);
        double f = radius_ * radius_ - vec.dot(vec);

        // Solving quadratic equation for t
        double aa = 1 - (b * b);
        double bb = 2 * (a - b * d);
        double cc = -(d * d) - f;
        double t = solve_quadratic(aa, bb, cc);

        if (t < min_dist || t > max_dist) {
            return -1;
        }
        // Check if cylinder is finite
        if (length_ > 0) {
            double alpha = d + t * b;
            if (alpha < -length_ || alpha > length_) {
                return -1;
            }
        }
        return t;
    }

    Vector3d calculate_normal_at_hit(const Vector3d& hit) const override {
        // N = Hit - [B . (Hit - A)] * B
        Vector3d v = hit - local_basis_.o;
        double alpha = local_basis_.vk.dot(v);
        Vector3d w = local_basis_.o + alpha * local_basis_.vk;
        Vector3d normal = hit - w;

        return normal * (1 / normal.norm());
    }

private:
    double radius_;
    double length_;
};


static std::shared_ptr<ActorBase> create_triangle(std::shared_ptr<cpptoml::table> items,
                                                  TextureFactory* texture_factory) {
    auto vertex_a = items->get_array_of<double>("A");
    if (!vertex_a) {
        //TODO
    }
    Vector3d A(vertex_a->data());

    auto vertex_b = items->get_array_of<double>("B");
    if (!vertex_b) {
        //TODO
    }
    Vector3d B(vertex_b->data());

    auto vertex_c = items->get_array_of<double>("C");
    if (!vertex_c) {
        //TODO
    }
    Vector3d C(vertex_c->data());

    auto tmp_color = items->get_array_of<double>("color");
    if (!tmp_color) {
        //TODO
    }
    Vector3d vec_color(tmp_color->data());
    TexturePixel triangle_color(vec_color);

    Vector3d vec_o = (A + B + C) / 3;
    Vector3d vec_i = B - A;
    Vector3d vec_k = vec_i.cross(C - B);
    Vector3d vec_j = vec_k.cross(vec_i);

    vec_i *= (1 / vec_i.norm());
    vec_j *= (1 / vec_j.norm());
    vec_k *= (1 / vec_k.norm());

    StandardBasis local_basis{vec_o, vec_i, vec_j, vec_k};
    double reflect_coef = items->get_as<double>("reflect").value_or(0);

    auto texture_mapper_ptr = create_texture_mapper(
                items, ActorType::Triangle, texture_factory);

    return std::shared_ptr<ActorBase>(
                new SimpleTriangle(local_basis, A, B, C, texture_mapper_ptr));
}


static std::shared_ptr<ActorBase> create_plane(std::shared_ptr<cpptoml::table> plane_items,
                                               TextureFactory* texture_factory) {
    auto plane_center = plane_items->get_array_of<double>("center");
    if (!plane_center) {
        //TODO
    }
    Vector3d plane_center_vec(plane_center->data());

    auto plane_normal = plane_items->get_array_of<double>("normal");
    if (!plane_normal) {
        //TODO
    }
    Vector3d plane_normal_vec(plane_normal->data());

    Vector3d fill_vec = fill_vector(plane_normal_vec);

    Vector3d plane_vec_i = fill_vec.cross(plane_normal_vec);
    Vector3d plane_vec_j = plane_normal_vec.cross(plane_vec_i);

    plane_vec_i *= (1 / plane_vec_i.norm());
    plane_vec_j *= (1 / plane_vec_j.norm());
    plane_normal_vec *= (1 / plane_normal_vec.norm());

    StandardBasis plane_basis{
        plane_center_vec,
        plane_vec_i,
        plane_vec_j,
        plane_normal_vec
    };

    auto texture_mapper_ptr = create_texture_mapper(
                plane_items, ActorType::Plane, texture_factory);

    auto plane_ptr = std::shared_ptr<ActorBase>(
                new SimplePlane(plane_basis, texture_mapper_ptr));

    return plane_ptr;
}


static std::shared_ptr<ActorBase> create_sphere(std::shared_ptr<cpptoml::table> sphere_items,
                                                TextureFactory* texture_factory) {
    auto sphere_center = sphere_items->get_array_of<double>("center");
    if (!sphere_center) {
        //TODO
    }
    Vector3d sphere_center_vec(sphere_center->data());

    Vector3d sphere_axis_vec(0, 0, 1);
    auto sphere_axis = sphere_items->get_array_of<double>("axis");
    if (sphere_axis) {
        Vector3d tmp_vec(sphere_axis->data());
        sphere_axis_vec = tmp_vec;
    }

    double sphere_radius = sphere_items->get_as<double>("radius").value_or(1);

    Vector3d fill_vec = fill_vector(sphere_axis_vec);

    Vector3d sphere_vec_i = fill_vec.cross(sphere_axis_vec);
    Vector3d sphere_vec_j = sphere_axis_vec.cross(sphere_vec_i);

    sphere_vec_i *= (1 / sphere_vec_i.norm());
    sphere_vec_j *= (1 / sphere_vec_j.norm());
    sphere_axis_vec *= (1 / sphere_axis_vec.norm());

    StandardBasis sphere_basis{
        sphere_center_vec,
        sphere_vec_i,
        sphere_vec_j,
        sphere_axis_vec
    };

    auto texture_mapper_ptr = create_texture_mapper(
                sphere_items, ActorType::Sphere, texture_factory);

    auto sphere_ptr = std::shared_ptr<ActorBase>(new SimpleSphere(
        sphere_basis,
        sphere_radius,
        texture_mapper_ptr
    ));

    return sphere_ptr;
}


static std::shared_ptr<ActorBase> create_cylinder(std::shared_ptr<cpptoml::table> cylinder_items,
                                                  TextureFactory* texture_factory) {
    auto cylinder_center = cylinder_items->get_array_of<double>("center");
    if (!cylinder_center) {
        //TODO
    }
    Vector3d cylinder_center_vec(cylinder_center->data());

    auto cylinder_direction = cylinder_items->get_array_of<double>("direction");
    if (!cylinder_direction) {
        //TODO
    }
    Vector3d cylinder_direction_vec(cylinder_direction->data());

    double cylinder_span = cylinder_items->get_as<double>("span").value_or(-1);
    double cylinder_radius = cylinder_items->get_as<double>("radius").value_or(1);

    Vector3d fill_vec = fill_vector(cylinder_direction_vec);

    Vector3d cylinder_vec_i = fill_vec.cross(cylinder_direction_vec);
    Vector3d cylinder_vec_j = cylinder_direction_vec.cross(cylinder_vec_i);

    cylinder_vec_i *= (1 / cylinder_vec_i.norm());
    cylinder_vec_j *= (1 / cylinder_vec_j.norm());
    cylinder_direction_vec *= (1 / cylinder_direction_vec.norm());

    StandardBasis cylinder_basis{
        cylinder_center_vec,
        cylinder_vec_i,
        cylinder_vec_j,
        cylinder_direction_vec
    };

    auto texture_mapper_ptr = create_texture_mapper(
                cylinder_items, ActorType::Cylinder, texture_factory);

    auto cylinder_ptr = std::shared_ptr<ActorBase>(new SimpleCylinder(
        cylinder_basis,
        cylinder_radius,
        cylinder_span,
        texture_mapper_ptr
    ));

    return cylinder_ptr;
}


static void create_cube_triangles(double s,
                                  const StandardBasis& face_basis,
                                  std::shared_ptr<TextureMapper> texture_mapper_ptr,
                                  std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) {
    Vector3d ta_A = face_basis.o + face_basis.vi * s + face_basis.vj * s;
    Vector3d ta_B = face_basis.o - face_basis.vi * s + face_basis.vj * s;
    Vector3d ta_C = face_basis.o + face_basis.vi * s - face_basis.vj * s;

    Vector3d tb_A = face_basis.o - face_basis.vi * s - face_basis.vj * s;
    Vector3d tb_B = face_basis.o + face_basis.vi * s - face_basis.vj * s;
    Vector3d tb_C = face_basis.o - face_basis.vi * s + face_basis.vj * s;

    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimpleTriangle(face_basis, ta_A, ta_B, ta_C, texture_mapper_ptr)));

    actor_ptrs->push_back(std::shared_ptr<ActorBase>(
                              new SimpleTriangle(face_basis, tb_A, tb_B, tb_C, texture_mapper_ptr)));
}


static void create_cube(TextureFactory* texture_factory,
                        std::shared_ptr<cpptoml::table> cube_items,
                        std::vector<std::shared_ptr<ActorBase>>* actor_ptrs) {
    auto cube_center = cube_items->get_array_of<double>("center");
    auto cube_direction = cube_items->get_array_of<double>("direction");

    double cube_scale = cube_items->get_as<double>("scale").value_or(1) / 2;

    auto texture_mapper_ptr = create_texture_mapper(
                cube_items, ActorType::Cube, texture_factory);  // for now only dummy textures

    if (!cube_center) {
        //TODO
    }
    Vector3d cube_vec_o(cube_center->data());

    if (!cube_direction) {
        //TODO
    }
    Vector3d cube_vec_k(cube_direction->data());

    Vector3d fill_vec = fill_vector(cube_vec_k);
    Vector3d cube_vec_i = fill_vec.cross(cube_vec_k);
    Vector3d cube_vec_j = cube_vec_k.cross(cube_vec_i);

    cube_vec_i *= (1 / cube_vec_i.norm());
    cube_vec_j *= (1 / cube_vec_j.norm());
    cube_vec_k *= (1 / cube_vec_k.norm());

    double angle_x = cube_items->get_as<double>("angle_x").value_or(0);
    Eigen::AngleAxisd m_x = Eigen::AngleAxisd(angle_x * M_PI / 180, Vector3d::UnitX());

    double angle_y = cube_items->get_as<double>("angle_y").value_or(0);
    Eigen::AngleAxisd m_y = Eigen::AngleAxisd(angle_y * M_PI / 180, Vector3d::UnitY());

    double angle_z = cube_items->get_as<double>("angle_z").value_or(0);
    Eigen::AngleAxisd m_z = Eigen::AngleAxisd(angle_z * M_PI / 180, Vector3d::UnitZ());

    Eigen::Matrix3d m_rot;
    m_rot = m_x * m_y * m_z;

    cube_vec_i = m_rot * cube_vec_i;
    cube_vec_j = m_rot * cube_vec_j;
    cube_vec_k = m_rot * cube_vec_k;

    Vector3d face_a_o = cube_vec_k * cube_scale + cube_vec_o;
    Vector3d face_b_o = -cube_vec_i * cube_scale + cube_vec_o;
    Vector3d face_c_o = -cube_vec_k * cube_scale + cube_vec_o;
    Vector3d face_d_o = cube_vec_i * cube_scale + cube_vec_o;
    Vector3d face_e_o = cube_vec_j * cube_scale + cube_vec_o;
    Vector3d face_f_o = -cube_vec_j * cube_scale + cube_vec_o;

    StandardBasis face_a_basis{face_a_o, cube_vec_i, cube_vec_j, cube_vec_k};
    StandardBasis face_b_basis{face_b_o, cube_vec_k, cube_vec_j, -cube_vec_i};
    StandardBasis face_c_basis{face_c_o, -cube_vec_i, cube_vec_j, -cube_vec_k};
    StandardBasis face_d_basis{face_d_o, -cube_vec_k, cube_vec_j, cube_vec_i};
    StandardBasis face_e_basis{face_e_o, -cube_vec_k, -cube_vec_i, cube_vec_j};
    StandardBasis face_f_basis{face_f_o, -cube_vec_k, cube_vec_i, -cube_vec_j};

    create_cube_triangles(cube_scale, face_a_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_b_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_c_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_d_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_e_basis, texture_mapper_ptr, actor_ptrs);
    create_cube_triangles(cube_scale, face_f_basis, texture_mapper_ptr, actor_ptrs);
}


void create_actors(ActorType actor_type,
                   TextureFactory* texture_factory,
                   std::shared_ptr<cpptoml::table> actor_items,
                   std::vector<std::shared_ptr<ActorBase>>* actor_ptrs)
{
    if (actor_type == ActorType::Plane)
        actor_ptrs->push_back(create_plane(actor_items, texture_factory));
    else if (actor_type == ActorType::Sphere)
        actor_ptrs->push_back(create_sphere(actor_items, texture_factory));
    else if (actor_type == ActorType::Cylinder)
        actor_ptrs->push_back(create_cylinder(actor_items, texture_factory));
    else if (actor_type == ActorType::Triangle)
        actor_ptrs->push_back(create_triangle(actor_items, texture_factory));
    else if (actor_type == ActorType::Cube)
        create_cube(texture_factory, actor_items, actor_ptrs);
}


}  // namespace mrtp
