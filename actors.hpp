/*
 *  Mrtp: A simple raytracing tool.
 *  Copyright (C) 2017  Mikolaj Feliks <mikolaj.feliks@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _ACTORS_H
#define _ACTORS_H

#include <cstdio>
#include <cmath>

#include "vector.hpp"
#include "color.hpp"
#include "texture.hpp"


#define TOLERANCE  0.000001

/*
 * Macros.
 */
#define IS_ZERO(x) ((x > -TOLERANCE) && (x < TOLERANCE))
#define IS_NOT_ZERO(x) ((x <= -TOLERANCE) || (x >= TOLERANCE))

/*
 * Macro to solve a quadratic equation for t.
 *
 * Since t is a scale in: P = O + t*D, return
 * only the smaller t and within the limits of (mint, maxt).
 *
 * Otherwise return -1.
 */
#define SOLVE_QUADRATIC(a, b, c, t, mint, maxt) \
    double delta; \
    delta = b * b - 4.0 * a * c; \
    if (delta < 0.0) { \
        t = -1.0; \
    } \
    else { \
        if IS_ZERO (delta) { \
            t = -b / (2.0 * a); \
        } \
        else { \
            double sqdelta, ta, tb; \
            sqdelta = sqrt (delta); \
            t  = 0.5 / a; \
            ta = (-b - sqdelta) * t; \
            tb = (-b + sqdelta) * t; \
            t  = (ta < tb) ? ta : tb; \
        } \
        if ((t < mint) || (t > maxt)) \
            t = -1.0; \
    }

class Sphere {
    Vector   center_;
    double   radius_;
    Color    color_;
    Sphere  *next_;

public:
    ~Sphere ();
    Sphere ();
    Sphere (Vector *center, double radius,
        Color *color);

    double Solve (Vector *origin, Vector *direction, 
        double mind, double maxd);
    void GetNormal (Vector *hit, Vector *normal);
    void DetermineColor (Vector *hit, Color *color);

    Sphere *GetNext ();
    void SetNext (Sphere *sphere);
};

class Plane {
    Vector   center_, normal_,
        texturex_, texturey_;
    Color    colora_, colorb_;
    double   scale_;
    Texture *texture_;
    Plane   *next_;

public:
    ~Plane ();
    Plane ();
    Plane (Vector *center, Vector *normal, Color *colora, 
        Color *colorb, double scale, Texture *texture);

    double Solve (Vector *origin, Vector *direction, 
        double mind, double maxd);
    void GetNormal (Vector *normal);
    void DetermineColor (Vector *hit, Color *color);

    Plane *GetNext ();
    void SetNext (Plane *plane);
};

class Cylinder {
    Vector    A_, B_;
    double    radius_, alpha_;
    Color     color_;
    Cylinder *next_;

public:
    ~Cylinder ();
    Cylinder ();
    Cylinder (Vector *a, Vector *b, double radius, 
        Color *color);

    double Solve (Vector *O, Vector *D,
        double mind, double maxd);
    void GetNormal (Vector *hit, Vector *normal);
    void DetermineColor (Vector *hit, Color *color);

    Cylinder *GetNext ();
    void SetNext (Cylinder *cylinder);
};

class Light {
    Vector  position_;

public:
    Light (Vector *origin);
    ~Light ();
    void GetToLight (Vector *hit, 
        Vector *tolight);
};


#endif /* _ACTORS_H */
