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
	return 0;
}
