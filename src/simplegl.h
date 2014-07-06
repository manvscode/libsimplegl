/* Copyright (C) 2013-2014 by Joseph A. Marrero, http://www.manvscode.com/
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
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
	#if GL_OES_vertex_array_object
	#define glBindVertexArray(array)        glBindVertexArrayOES(array) 
	#define glDeleteVertexArrays(n, arrays) glDeleteVertexArraysOES(n, arrays)
	#define glGenVertexArrays(n, arrays)    glGenVertexArraysOES(n, arrays)  
	#define glIsVertexArray(array)          glIsVertexArrayOES(array)
	#endif
	#else
	#include <OpenGL/gl3.h>
	#include <OpenGL/gl3ext.h>
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
#include <lib3dmath/vec2.h>
#include <lib3dmath/vec3.h>
#include <lib3dmath/vec4.h>
#include <lib3dmath/mat2.h>
#include <lib3dmath/mat3.h>
#include <lib3dmath/mat4.h>
#include <lib3dmath/quat.h>
#include <math.h>

/*
 * Projections
 */
mat4_t orthographic       ( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far );
mat4_t frustum            ( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far );
mat4_t perspective        ( GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far );
vec4_t viewport_unproject ( const vec2_t* position, const mat4_t* projection, const mat4_t* model );
vec2_t viewport_project   ( const vec4_t* point, const mat4_t* projection, const mat4_t* model );

/*
 * Transformations
 */
mat4_t translate     ( const vec3_t* t );
mat4_t scale         ( const vec3_t* s );
mat4_t uniform_scale ( GLfloat scale );
mat4_t rotate_x      ( GLfloat angle );
mat4_t rotate_y      ( GLfloat angle );
mat4_t rotate_z      ( GLfloat angle );
mat4_t rotate_xyz    ( const GLchar* order, ... );
mat4_t orientation   ( vec3_t* forward, vec3_t* left, vec3_t* up );
mat4_t look_at       ( const pt3_t* eye, const pt3_t* target, const vec3_t* up );

/*
 * Texturing
 */
#define TEX_CLAMP_S         (0x01) /* not repeating along s */
#define TEX_CLAMP_T         (0x02) /* not repeating along t */
#define TEX_CLAMP_R         (0x04) /* not repeating along r */
#define TEX_COMPRESS        (0x08)
#define TEX_BORDER          (0x10)
#define TEX_COMPRESS_RGB    (0x20)
#define TEX_COMPRESS_RGBA   (0x40)

GLuint tex_create               ( void );
void   tex_destroy              ( GLuint texture );
void   tex_setup_texture        ( GLuint texture, GLsizei width, GLsizei height, GLsizei depth, GLbyte bit_depth, const GLvoid* pixels,
                                  GLint min_filter, GLint mag_filter, GLubyte flags, GLuint texture_dimensions );
bool   tex_load_1d              ( GLuint texture, const GLchar* filename, GLint min_filter, GLint mag_filter, GLubyte flags );
bool   tex_load_2d              ( GLuint texture, const GLchar* filename, GLint min_filter, GLint mag_filter, GLubyte flags );
bool   tex_load_2d_with_linear  ( GLuint texture, const GLchar* filename, GLubyte flags );
bool   tex_load_2d_with_mipmaps ( GLuint texture, const GLchar* filename, GLubyte flags );
bool   tex_load_3d              ( GLuint texture, const GLchar* filename, GLsizei voxel_bit_depth,
                                  GLsizei width, GLsizei height, GLsizei length, GLint min_filter, GLint mag_filter, GLubyte flags );
bool   tex_cube_map_setup       ( GLuint texture, const GLchar* xpos, const GLchar* xneg, const GLchar* ypos, const GLchar* yneg, const GLchar* zpos, const GLchar* zneg );

/*
 * Shaders
 */
typedef struct shader_info {
	GLenum type; /* GL_VERTEX_SHADER | GL_FRAGMENT_SHADER */
	const GLchar* filename;
} shader_info_t;
#define shader_info_count( array )  (sizeof(array) / sizeof(array[0]))

GLboolean     glsl_program_from_shaders      ( GLuint* p_program, const shader_info_t* shaders, GLsizei count, GLchar** shader_log, GLchar** program_log );
GLboolean     glsl_program_create            ( GLuint* p_program, const GLuint *p_shaders, GLsizei shader_count, GLboolean mark_shaders_for_deletion, GLchar** log );
GLboolean     glsl_shader_create_from_source ( GLuint* p_shader, GLenum type /* GL_VERTEX_SHADER | GL_FRAGMENT_SHADER */, const GLchar* source, GLchar** log );
GLuint        glsl_create                    ( GLenum type );
GLboolean     glsl_destroy                   ( GLuint object /* program or shader */ );
GLchar*       glsl_shader_load               ( const GLchar* path );
GLboolean     glsl_shader_compile            ( GLuint shader, const GLchar* source );
GLboolean     glsl_attach_shader             ( GLuint program, GLuint shader );
GLboolean     glsl_link_program              ( GLuint program );
GLint         glsl_bind_attribute            ( GLuint program, const GLchar* name );
GLint         glsl_bind_uniform              ( GLuint program, const GLchar* name );
GLchar*       glsl_log                       ( GLuint object /* program or shader */ );
const GLchar* glsl_object_type_string        ( GLenum type );
const GLchar* glsl_shader_version_code       ( const GLchar* maxVersion );

/*
 * Buffers
 */
GLboolean buffer_create  ( GLuint* id, const GLvoid* geometry, GLsizei element_size, GLsizei count, GLenum target /*GL_ARRAY_BUFFER*/, GLenum usage /*GL_STATIC_DRAW*/ );
GLboolean buffer_destroy ( const GLuint* id );

/*
 * Cameras
 */
struct camera;
typedef struct camera camera_t;

camera_t*     camera_create             ( GLint screen_width, GLint screen_height, GLfloat near, GLfloat far, GLfloat fov, const pt3_t* position );
void          camera_destroy            ( camera_t* camera );
const mat4_t* camera_projection_matrix  ( const camera_t* camera );
const mat4_t* camera_model_matrix       ( const camera_t* camera );
const mat4_t* camera_orientation_matrix ( const camera_t* camera );
mat3_t        camera_normal_matrix      ( const camera_t* camera );
mat4_t        camera_view_matrix        ( const camera_t* camera );
vec3_t        camera_forward_vector     ( const camera_t* camera );
vec3_t        camera_up_vector          ( const camera_t* camera );
vec3_t        camera_side_vector        ( const camera_t* camera );
void          camera_set_perspective    ( camera_t* camera, GLint screen_width, GLint screen_height, GLfloat near, GLfloat far, GLfloat fov );
void          camera_set_position       ( camera_t* camera, const pt3_t* position );
const vec3_t* camera_position           ( const camera_t* camera );
void          camera_offset_orientation ( camera_t* camera, GLfloat xangle, GLfloat yangle );
void          camera_move_forwards      ( camera_t* camera, GLfloat a );
void          camera_move_sideways      ( camera_t* camera, GLfloat a );
void          camera_update             ( camera_t* camera, GLfloat delta );

/*
 * Fonts
 */
struct raster_font;
typedef struct raster_font raster_font_t;

#define RASTER_FONT_VINCENT_8X8      (0)
#define RASTER_FONT_FONT1_8X8        (1)
#define RASTER_FONT_FONT2_8X8        (2)
#define RASTER_FONT_FONT3_16X16      (3)

raster_font_t* raster_font_create          ( GLushort type );
void           raster_font_destroy         ( raster_font_t* fnt );
GLushort       raster_font_glyph_width     ( const raster_font_t* fnt );
GLushort       raster_font_glyph_height    ( const raster_font_t* fnt );
void           raster_font_write           ( const raster_font_t* fnt, const vec2_t* position, const vec3_t* color, GLfloat size, const char* text );
void           raster_font_writef          ( const raster_font_t* fnt, const vec2_t* position, const vec3_t* color, GLfloat size, const char* format, ... );
void           raster_font_shadowed_writef ( const raster_font_t* fnt, const vec2_t* position, const vec3_t* color, const vec3_t* shadow, GLfloat size, const char* format, ... );

/*
 * Overlays
 */
bool overlay_initialize   ( void );
void overlay_deinitialize ( void );
void overlay_render       ( const vec2_t* position, const vec2_t* size, const vec3_t* color, GLuint texture );

/*
 * Polyhedra
 */
typedef struct polyhedra {
	GLfloat* vertices;
	GLsizei vertices_count;
	GLushort* indices;
	GLsizei indices_count;
	GLfloat* tex_coords;
	GLsizei tex_coords_count;
	GLfloat* colors;
	GLsizei colors_count;
} polyhedra_t;

void      polyhedra_create  ( polyhedra_t* polyhedra );
void      polyhedra_destroy ( polyhedra_t* polyhedra );
GLboolean tetrahedron       ( polyhedra_t* polyhedra, GLfloat scale, bool withColors );
GLboolean cube              ( polyhedra_t* polyhedra, GLfloat scale, bool withColors );


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
 * Extensions
 */
bool  gl_has_extension  ( const GLchar* ext );
void* gl_extension      ( const GLchar* procName );

/*
 * Miscellaneous
 */ 
const GLchar* gl_vendor         ( void );
const GLchar* gl_renderer       ( void );
const GLchar* gl_version        ( void );
const GLchar* gl_shader_version ( void );
void          gl_info_print     ( void );
GLenum        gl_error          ( void );
GLuint        frame_delta       ( GLuint now /* milliseconds */ );
GLfloat       frame_rate        ( GLuint delta /* milliseconds */ );
void          frame_rate_print  ( GLuint delta /* milliseconds */ );

#ifdef SIMPLEGL_DEBUG
#define GL_ASSERT_NO_ERROR()    assert(gl_error() == GL_NO_ERROR);
#else
#define GL_ASSERT_NO_ERROR()
#endif

#ifdef __cplusplus
} /* C linkage */
#endif
#endif /* _SIMPLEGL_H_ */
