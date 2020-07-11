/* File      : world.hpp
 * Program   : mrtp
 * Copyright : Mikolaj Feliks  <mikolaj.feliks@gmail.com>
 * License   : LGPL v3  (http://www.gnu.org/licenses/gpl-3.0.en.html)
 */
#ifndef _WORLD_H
#define _WORLD_H

#include <memory>
#include <vector>
#include <list>
#include "cpptoml.h"

#include "actors.hpp"
#include "camera.hpp"
#include "light.hpp"


namespace mrtp {

enum WorldStatus_t {ws_ok, ws_no_file, ws_parse_error, ws_no_camera, 
                    ws_no_light, ws_no_actors, ws_camera_param, 
                    ws_light_param, ws_plane_param, ws_sphere_param, 
                    ws_cylinder_param, ws_plane_texture, ws_sphere_texture, 
                    ws_cylinder_texture};


class World {
  public:
    World(const char *path);
    ~World();
    WorldStatus_t initialize();

    Camera *ptr_camera_;
    Light *ptr_light_;
    std::vector<Actor *> ptr_actors_;

  private:
    WorldStatus_t load_plane(std::shared_ptr<cpptoml::table> items);
    WorldStatus_t load_sphere(std::shared_ptr<cpptoml::table> items);
    WorldStatus_t load_cylinder(std::shared_ptr<cpptoml::table> items);
    
    WorldStatus_t load_planes(std::shared_ptr<cpptoml::table_array> array);
    WorldStatus_t load_spheres(std::shared_ptr<cpptoml::table_array> array);
    WorldStatus_t load_cylinders(std::shared_ptr<cpptoml::table_array> array);

    const char *path_;
    std::list<Camera> cameras_;
    std::list<Light> lights_;
    std::list<Plane> planes_;
    std::list<Sphere> spheres_;
    std::list<Cylinder> cylinders_;
};

} //namespace mrtp

#endif //_WORLD_H
