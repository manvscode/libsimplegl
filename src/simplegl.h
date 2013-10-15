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
#ifndef _SIMPLEGL_H_
#define _SIMPLEGL_H_
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <assert.h>
/*
 * Mathematics
 */
#include <lib3dmath/mathematics.h>
#include <lib3dmath/vec2.h>
#include <lib3dmath/vec3.h>
#include <lib3dmath/vec4.h>
#include <lib3dmath/mat2.h>
#include <lib3dmath/mat3.h>
#include <lib3dmath/mat4.h>
#include <math.h>
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#else
#error "Need a C99 compiler."
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

/*
 * Always include OpenGL and GLU headers
 */
#if _WIN64
	#error "Not supported yet."
#elif _WIN32
	#error "Not supported yet."
#elif __APPLE__
	#if defined(SIMPLEGL_IOS)
	#include <OpenGLES/ES2/gl.h>
	//#include <OpenGLES/ES2/glext.h>
	#if GL_OES_vertex_array_object
	#define glBindVertexArray(array)        glBindVertexArrayOES(array) 
	#define glDeleteVertexArrays(n, arrays) glDeleteVertexArraysOES(n, arrays)
	#define glGenVertexArrays(n, arrays)    glGenVertexArraysOES(n, arrays)  
	#define glIsVertexArray(array)          glIsVertexArrayOES(array)  
	#define glClearColor                    glClearColorf
	#endif
	#else
	#include <OpenGL/gl3.h>
	#endif
#else 
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#ifndef GL_PROGRAM
#define GL_PROGRAM  0
#endif

/*
 * Mathematics
 */
#include <lib3dmath/mathematics.h>
#include <math.h>

/*
 * Texturing
 */
GLuint tex2d_create        ( void );
void   tex2d_destroy       ( GLuint texture );
bool   tex2d_load          ( GLuint texture, const char* filename, GLint min_filter, GLint mag_filter, bool clamp );
bool   tex2d_load_for_2d   ( GLuint texture, const char* filename );
bool   tex2d_load_for_3d   ( GLuint texture, const char* filename, bool clamp );
void   tex2d_setup_texture ( GLuint texture, GLsizei width, GLsizei height, GLbyte bit_depth, const void* pixels, GLint min_filter, GLint mag_filter, bool clamp );

/*
 * Projections
 */
mat4_t orthographic ( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far );
mat4_t frustum      ( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far );
mat4_t perspective  ( GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far );
mat4_t look_at      ( const pt3_t* p, const pt3_t* t, const vec3_t* u );

/*
 * Transformations
 */
mat4_t translate     ( const vec3_t* t );
mat4_t scale         ( const vec3_t* s );
mat4_t uniform_scale ( GLfloat scale );
mat4_t rotate_x      ( GLfloat angle );
mat4_t rotate_y      ( GLfloat angle );
mat4_t rotate_z      ( GLfloat angle );
mat4_t rotate_xyz    ( const char* order, ... );
mat4_t orientation   ( vec3_t* forward, vec3_t* left, vec3_t* up );

/*
 * Shaders
 */
typedef struct shader_info {
	GLenum type; /* GL_VERTEX_SHADER | GL_FRAGMENT_SHADER */
	const char* filename;
} shader_info_t;
#define shader_info_count( array )  (sizeof(array) / sizeof(array[0]))

GLboolean     glsl_program_from_shaders      ( GLuint* p_program, const shader_info_t* shaders, GLsizei count, GLchar** shader_log, GLchar** program_log );
GLboolean     glsl_program_create            ( GLuint* p_program, const GLuint *p_shaders, GLsizei shader_count, GLboolean mark_shaders_for_deletion, GLchar** log );
GLboolean     glsl_shader_create_from_source ( GLuint* p_shader, GLenum type /* GL_VERTEX_SHADER | GL_FRAGMENT_SHADER */, const GLchar* source, GLchar** log );
GLuint        glsl_create                    ( GLenum type );
GLboolean     glsl_destroy                   ( GLuint object /* program or shader */ );
GLchar*       glsl_shader_load               ( const char* path );
GLboolean     glsl_shader_compile            ( GLuint shader, const GLchar* source );
GLboolean     glsl_attach_shader             ( GLuint program, GLuint shader );
GLboolean     glsl_link_program              ( GLuint program );
GLint         glsl_bind_attribute            ( GLuint program, const GLchar* name );
GLint         glsl_bind_uniform              ( GLuint program, const GLchar* name );
GLchar*       glsl_log                       ( GLuint object /* program or shader */ );

/*
 * Buffers
 */
GLboolean buffer_create  ( GLuint* id, const void* geometry, size_t element_size, size_t count, GLenum target /*GL_ARRAY_BUFFER*/, GLenum usage /*GL_STATIC_DRAW*/ );
GLboolean buffer_destroy ( const GLuint* id );

/*
 * Objects
 */
typedef struct polyhedra {
	GLfloat* vertices;
	size_t vertices_count;
	GLushort* indices;
	size_t indices_count;
	GLfloat* tex_coords;
	size_t tex_coords_count;
	GLfloat* colors;
	size_t colors_count;
} polyhedra_t;

void      polyhedra_create  ( polyhedra_t* polyhedra );
void      polyhedra_destroy ( polyhedra_t* polyhedra );
GLboolean tetrahedron       ( polyhedra_t* polyhedra, GLfloat scale );
GLboolean cube              ( polyhedra_t* polyhedra, GLfloat scale );

/*
 * 3D Axes Helper
 * (x-axis,y-axis,z-axis) --> (red,green,blue)
 */
struct axes_3d;
typedef struct axes_3d* axes_3d_t;

axes_3d_t axes_3d_create  ( GLfloat line_width );
void      axes_3d_destroy ( axes_3d_t* axes );
void      axes_3d_render  ( axes_3d_t axes, const GLfloat* model_view );

/*
 * Miscellaneous
 */ 
void      dump_gl_info ( void );
GLenum    check_gl     ( void );

#ifdef SIMPLEGL_DEBUG
#define GL_ASSERT_NO_ERROR()    assert(check_gl() == GL_NO_ERROR);
#else
#define GL_ASSERT_NO_ERROR()
#endif

#ifdef __cplusplus
} /* C linkage */
#endif
#endif /* _SIMPLEGL_H_ */
