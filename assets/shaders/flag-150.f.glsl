#version 400

in vec3 f_normal;
in vec2 f_tex_coord;
in vec4 f_color;

uniform mat3 u_normal_matrix;
uniform sampler2D u_texture;

out vec4 color;

const vec3 light_direction = vec3(0,0,1);

void main( ) {
	// Convert normal and position to eye coords
	vec3 tnorm = normalize( u_normal_matrix * f_normal );
	vec3 light_direction_eye_space = u_normal_matrix * light_direction;

	vec3 diffuse_lighting = vec3(1,1,1) * max( dot(light_direction_eye_space, tnorm), 0.0f );
	vec4 light_intensity = vec4( diffuse_lighting, 1.0f );

	color = f_color * texture( u_texture, f_tex_coord );
	//color = texture( u_texture, f_tex_coord ) * light_intensity;
}
