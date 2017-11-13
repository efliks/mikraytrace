/*
 *  Mrtp: A simple raytracing tool.
 *  Copyright (C) 2017  Mikolaj Feliks <mikolaj.feliks@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _VECTOR_H
#define _VECTOR_H

#include <cmath>
/*
#include <iomanip>
#include <iostream>
*/


class Vector {
    double  x_, y_, z_;

public:
    ~Vector ();
    Vector ();
    Vector (const double *flat);
    Vector (const double x, const double y, const double z);

    void Set (const double *flat);
    void Set (const double x, const double y, const double z);
    void Get (double *flat) const;
    void Get (double *x, double *y, double *z) const;

    void Scale_InPlace (const double scale);
    void Normalize_InPlace ();

    void CopyTo (Vector *other) const;
    void Print () const;
    double Magnitude () const;

    Vector Reflect (const Vector *other) const;
    Vector GenerateUnitVector () const;

    Vector operator+ (const Vector &other) const;
    Vector operator- (const Vector &other) const;
    Vector operator* (const double scale) const;

    double operator* (const Vector &other) const;
    Vector operator^ (const Vector &other) const;
};

#endif /* _VECTOR_H */
