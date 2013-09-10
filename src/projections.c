#include <math.h>
#include <assert.h>
#include "mathematics.h"
#include "simplegl.h"


mat4_t orthographic( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far )
{
	mat4_t projection = MAT4_MATRIX(
		2.0 / (right - left)          , 0.0                           ,  0.0                      , 0.0,
		0.0                           , 2.0 / (top - bottom)          ,  0.0                      , 0.0,
		0.0                           , 0.0                           , -2.0 / (far - near)       , 0.0,
		-(right + left)/(right - left), -(top + bottom)/(top - bottom), -(far + near)/(far - near), 1.0
	);
	return projection;
}

mat4_t frustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far )
{
	GLdouble A = 2.0 * near / (right - left);
	GLdouble B = (right + left) / (right - left);
	GLdouble C = 2.0 * near / (top - bottom);
	GLdouble D = (top + bottom) / (top - bottom);
	GLdouble E = -(far + near) / (far - near);
	GLdouble F = -(2.0 * far * near) / (far - near);

	mat4_t projection = MAT4_MATRIX(
		  A, 0.0,   B, 0.0,
		0.0,   C,   D, 0.0,
		0.0, 0.0,   E,   F,
		0.0, 0.0,-1.0, 0.0
	);
	return projection;
}


mat4_t perspective( GLdouble fov, GLdouble aspect, GLdouble near, GLdouble far )
{
	GLdouble A = 1.0 / tan(fov * 0.5 * RADIANS_PER_DEGREE);
	GLdouble B = -far / (far - near);
	GLdouble C = -(far * near)/ (far - near);

	mat4_t projection = MAT4_MATRIX(
		  A, 0.0, 0.0, 0.0,
		0.0,   A, 0.0, 0.0,
		0.0, 0.0,   B,-1.0,
		0.0, 0.0,   C, 0.0
	);
	return projection;
	//return MAT4_IDENTITY;
}

mat4_t look_at( const pt3_t* p, const pt3_t* t, const vec3_t* u )
{
	vec3_t z = VEC3_VECTOR( t->x - p->x, t->y - p->y, t->z - p->z );
	vec3_normalize( &z );

	vec3_t x = vec3_cross_product( &z, u );
	vec3_normalize( &x );

	vec3_t y = vec3_cross_product( &z, &x );
	vec3_normalize( &y );

	mat4_t projection = MAT4_MATRIX(
		  x.x,   x.y,   x.z, 0.0, /* x-axis */
		  y.x,   y.y,   y.z, 0.0, /* y-axis */
		  z.x,   z.y,   z.z, 0.0, /* z-axis */
		-p->x, -p->y, -p->z, 1.0  /* translation */
	);
	return projection;
}

