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
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#ifdef __restrict
#undef __restrict
#define __restrict restrict
#endif
#ifdef __inline
#undef __inline
#define __inline inline
#endif
#else
#define bool int
#define true 1
#define false 0
#ifdef __restrict
#undef __restrict
#define __restrict
#endif
#ifdef __inline
#undef __inline
#define __inline
#endif
#endif
#include <assert.h>

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

/*
 * Always include OpenGL and GLU headers
 */
#if __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
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
mat4_t orthographic ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far );
mat4_t frustum      ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far );
mat4_t perspective  ( GLdouble fov, GLdouble aspect, GLdouble near, GLdouble far );
mat4_t look_at      ( const pt3_t* p, const pt3_t* t, const vec3_t* u );

/*
 * Transformations
 */
mat4_t translate     ( const vec3_t* t );
mat4_t scale         ( const vec3_t* s );
mat4_t uniform_scale ( GLdouble scale );
mat4_t rotate_x      ( GLdouble angle );
mat4_t rotate_y      ( GLdouble angle );
mat4_t rotate_z      ( GLdouble angle );
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
GLboolean     glsl_program_create            ( GLuint* p_program, GLuint *p_shaders, GLsizei shader_count, GLboolean mark_shaders_for_deletion );
GLboolean     glsl_shader_create_from_source ( GLuint* p_shader, GLenum type /* GL_VERTEX_SHADER | GL_FRAGMENT_SHADER */, const GLchar* source );
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
