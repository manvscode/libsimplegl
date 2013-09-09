#include <stdio.h>
#include <stdlib.h>
#include <vec2.h>
#include <vec3.h>
#include <mat2.h>
#include <mat3.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif






int main( int argc, char* argv[] )
{
	mat3_t matrix = MAT3_IDENITY;

	printf( "x' = %s\n", vec3_to_string( mat3_x_vector(&matrix) ) );
	printf( "y' = %s\n", vec3_to_string( mat3_y_vector(&matrix) ) );
	printf( "z' = %s\n", vec3_to_string( mat3_z_vector(&matrix) ) );

	printf( "%s\n", mat3_to_string(&matrix) );


	mat2_t A = MAT2_MATRIX(2, 1, 0.5, 3);
	mat2_t B = MAT2_MATRIX(0.33, 7, 4, -3);

	mat2_t C = mat2_mult_matrix( &A, &B );

	printf( "%s\n", mat2_to_string(&C) );
	printf( "x' = %s\n", vec2_to_string( mat2_x_vector(&C) ) );
	printf( "y' = %s\n", vec2_to_string( mat2_y_vector(&C) ) );


#if 1
	mat3_t D = MAT3_MATRIX(1, 0, 2, 3, 1, 1, 1, 5, 1);
	mat3_t E = MAT3_MATRIX(1, 1, 1, 1, 1, 1, 1, 1, 1);
	vec3_t a = VEC3_VECTOR(1,2,1);
	vec3_t r = mat3_mult_vector( &D, &a );



	printf( "det D = %lf\n", mat3_determinant(&D) );

	printf( "%s\n", mat3_to_string(&D) );
	printf( "%s\n", vec3_to_string(&a) );
	printf( "%s\n", vec3_to_string(&r) );

	mat3_t s = mat3_mult_matrix( &D, &E );
	printf( "%s\n", mat3_to_string(&s) );
	mat3_transpose( &s );
	printf( "%s\n", mat3_to_string(&s) );
#endif

	return 0;
}
