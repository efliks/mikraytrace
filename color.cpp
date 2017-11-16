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
#include "color.hpp"

const real_t  kConvertToByte = 255.0f;
const real_t  kConvertToFloat = 1.0f / kConvertToByte;


Color::~Color () {
}

Color::Color () {
}

Color::Color (const real_t red, const real_t green, 
              const real_t blue) {
    red_   = red;
    green_ = green;
    blue_  = blue;
}

Color::Color (const unsigned char red, 
              const unsigned char green, const unsigned char blue) {
    red_   = (real_t) red   * kConvertToFloat;
    green_ = (real_t) green * kConvertToFloat;
    blue_  = (real_t) blue  * kConvertToFloat;
}

void Color::Set (const real_t red, const real_t green, 
                 const real_t blue) {
    red_   = red;
    green_ = green;
    blue_  = blue;
}

void Color::Set (const unsigned char red, 
                 const unsigned char green, const unsigned char blue) {
    red_   = (real_t) red   * kConvertToFloat;
    green_ = (real_t) green * kConvertToFloat;
    blue_  = (real_t) blue  * kConvertToFloat;
}

void Color::Get (unsigned char *red, unsigned char *green, 
                 unsigned char *blue) const {
    *red   = (unsigned char) (kConvertToByte * red_);
    *green = (unsigned char) (kConvertToByte * green_);
    *blue  = (unsigned char) (kConvertToByte * blue_);
}

void Color::Get (real_t *red, real_t *green, real_t *blue) const {
    *red   = red_;
    *green = green_;
    *blue  = blue_;
}

void Color::CopyTo (Color *other) const {
    other->red_   = red_;
    other->green_ = green_;
    other->blue_  = blue_;
}

void Color::Scale_InPlace (real_t scale) {
    /*
    static const real_t min = 0.0f, max = 1.0f;

    scale = (scale < min) ? min : ((scale > max) ? max : scale);
    */
    red_   = red_   * (real_t) scale;
    green_ = green_ * (real_t) scale;
    blue_  = blue_  * (real_t) scale;
}

void Color::Combine_InPlace (const Color *other, const real_t lambda) {
    real_t complement = 1.0f - lambda;

    red_   = red_   * complement + other->red_   * lambda;
    green_ = green_ * complement + other->green_ * lambda;
    blue_  = blue_  * complement + other->blue_  * lambda;
}

void Color::Zero () {
    red_   = 0.0f;
    green_ = 0.0f;
    blue_  = 0.0f;
}
