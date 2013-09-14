#include <stdarg.h>
#include <assert.h>
#include "math.h"
#include "simplegl.h"

mat4_t translate( const vec3_t* t )
{
	mat4_t transform = MAT4_MATRIX(
		 1.0,  0.0,  0.0,  0.0,
		 0.0,  1.0,  0.0,  0.0,
		 0.0,  0.0,  1.0,  0.0,
	 	t->x, t->y, t->z,  1.0
	);
	return transform;
}

mat4_t scale( const vec3_t* s )
{
	mat4_t transform = MAT4_MATRIX(
		s->x,  0.0,  0.0,  0.0,
		 0.0, s->y,  0.0,  0.0,
	 	 0.0,  0.0, s->z,  0.0,
	 	 0.0,  0.0,  0.0,  1.0
	);
	return transform;
}

mat4_t uniform_scale( GLdouble s )
{
	mat4_t transform = MAT4_MATRIX(
		  s, 0.0, 0.0, 0.0,
		0.0,   s, 0.0, 0.0,
	 	0.0, 0.0,   s, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
	return transform;
}

mat4_t rotate_x( GLdouble a )
{
	GLdouble s = sin( a );
	GLdouble c = cos( a );

	mat4_t transform = MAT4_MATRIX(
		1.0, 0.0, 0.0, 0.0,
		0.0,   c,  -s, 0.0,
	 	0.0,   s,   c, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
	return transform;
}

mat4_t rotate_y( GLdouble a )
{
	GLdouble s = sin( a );
	GLdouble c = cos( a );

	mat4_t transform = MAT4_MATRIX(
		  c, 0.0,   s, 0.0,
		0.0, 1.0, 0.0, 0.0,
	 	 -s, 0.0,   c, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
	return transform;
}

mat4_t rotate_z( GLdouble a )
{
	GLdouble s = sin( a );
	GLdouble c = cos( a );

	mat4_t transform = MAT4_MATRIX(
		  c,  -s, 0.0, 0.0,
		  s,   c, 0.0, 0.0,
	 	0.0, 0.0, 1.0, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
	return transform;
}

mat4_t rotate_xyz( const char* order, ... )
{
	va_list list;
	va_start( list, order );
	mat4_t result = MAT4_IDENTITY;

	while( *order )
	{
		switch( *order++ )
		{
			case 'x':
			case 'X':
			{
				GLdouble angle = va_arg( list, GLdouble );
				mat4_t r = rotate_x( angle );
				result = mat4_mult_matrix( &result, &r );
				break;
			}
			case 'y':
			case 'Y':
			{
				GLdouble angle = va_arg( list, GLdouble );
				mat4_t r = rotate_y( angle );
				result = mat4_mult_matrix( &result, &r );
				break;
			}
			case 'z':
			case 'Z':
			{
				GLdouble angle = va_arg( list, GLdouble );
				mat4_t r = rotate_z( angle );
				result = mat4_mult_matrix( &result, &r );
				break;
			}
			default:
				break;
		}
	}

	va_end( list );
	return result;
}

/*
mat4_t rotate_xyz( GLdouble xa, GLdouble ya, GLdouble za )
{
	mat4_t result = MAT4_IDENTITY;

	if( abs(xa) < SCALAR_EPSILON )
	{
		mat4_t rx = rotate_x( xa );
		mat4_mult_matrix( result, &rx );
	}

	if( abs(ya) < SCALAR_EPSILON )
	{
		mat4_t ry = rotate_y( ya );
		mat4_mult_matrix( result, &ry );
	}

	if( abs(za) < SCALAR_EPSILON )
	{
		mat4_t rz = rotate_z( za );
	}
}
*/

mat4_t orientation( vec3_t* f, vec3_t* l, vec3_t* u )
{
	vec3_normalize( f );
	vec3_normalize( l );
	vec3_normalize( u );

	mat4_t transform = MAT4_MATRIX(
		l->x, l->y, l->z, 0.0,
		u->x, u->y, u->z, 0.0,
		f->x, f->y, f->z, 0.0,
		0.0,   0.0,  0.0, 1.0
	);
	return transform;
}
