/* Copyright (C) 2013 by Joseph A. Marrero, http://www.manvscode.com/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <math.h>
#include <assert.h>
#include "math.h"

const vec3_t* mat3_x_vector( const mat3_t* m )
{
    return (vec3_t*) &m->x;
}

const vec3_t* mat3_y_vector( const mat3_t* m )
{
    return (vec3_t*) &m->y;
}

const vec3_t* mat3_z_vector( const mat3_t* m )
{
    return (vec3_t*) &m->z;
}

const vec3_t mat3_multiply( const mat3_t* m, const vec3_t* v )
{
    vec3_t result;
    // TODO: Implement this!
    assert( false );
    return result;
}

void mat3_invert( mat3_t* m )
{
    // TODO: Implement this!
    assert( false );
}

void mat3_transpose( mat3_t* m )
{
}
