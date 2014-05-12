#version 150

in vec3 f_tex_coord;
//in vec4 f_color;
uniform sampler3D u_voxel_data;
uniform sampler2D u_back_voxels;
uniform sampler1D u_color_transfer;
uniform uint u_render_pass;

out vec4 color;

const uint RENDER_PASS_BACK_VOXELS    = uint(0);
const uint RENDER_PASS_SAMPLED_VOXELS = uint(1);
const float STEP_SIZE = 0.008f;

vec3 color_transfer_function( float sample )
{
	//return vec3( sample );
	return texture( u_color_transfer, sample, 0 ).rgb;
}

float alpha_transfer_function( float sample )
{
	if( sample > 0.1f ) return 1.0f;
	else return 0.0f;
	//return texture( u_color_transfer, sample, 0 ).b;
}

void render_back_voxels( )
{
	/* Render texture coordinates to an offscreen texture 
	 * to be used on the next rendering phase.
	 */
	color = vec4( f_tex_coord, 1 );
}

void render_sampled_voxels_xray( )
{
	vec3 endVoxel   = texelFetch( u_back_voxels, ivec2(gl_FragCoord.xy), 0 ).rgb;
	vec3 startVoxel = f_tex_coord;

	vec3 maxRay        = endVoxel - startVoxel;
	float maxRayLength = length( maxRay );

	vec3 normalizedRay = normalize(maxRay.xyz);
	vec3 stepVector    = normalizedRay * STEP_SIZE;

	vec3 currentRay        = vec3( 0.0f );
	float currentRayLength = 0.0f;
	float sample           = 0.0f;
	vec3 accumulatedColor  = vec3( 0.0f );
	float accumulatedAlpha = 0.0f;

	while( currentRayLength < maxRayLength && accumulatedAlpha < 1.0f )
	{
		sample = texture( u_voxel_data, currentRay + f_tex_coord ).r;

		vec3 colorSample  = color_transfer_function( sample );
		float alphaSample = alpha_transfer_function( sample ) * STEP_SIZE;

		accumulatedColor += (1.0f - accumulatedAlpha) * (colorSample * alphaSample);
		accumulatedAlpha += alphaSample;

		currentRay       += stepVector;
		currentRayLength += STEP_SIZE;
	}


	color.rgb = accumulatedColor;
	color.a   = accumulatedAlpha;
}

void render_sampled_voxels_high_intensity_projection( )
{
	vec3 endVoxel   = texelFetch( u_back_voxels, ivec2(gl_FragCoord.xy), 0 ).rgb;
	vec3 startVoxel = f_tex_coord;

	vec3 maxRay        = endVoxel - startVoxel;
	float maxRayLength = length( maxRay );

	vec3 normalizedRay = normalize(maxRay.xyz);
	vec3 stepVector    = normalizedRay * STEP_SIZE;

	vec3 currentRay        = vec3( 0.0f );
	float currentRayLength = 0.0f;
	float maxSample        = 0.0f;

	while( currentRayLength < maxRayLength )
	{
		float sample = texture( u_voxel_data, currentRay + f_tex_coord ).r;
		if( sample > maxSample ) maxSample = sample;	

		currentRay       += stepVector;
		currentRayLength += STEP_SIZE;
	}

	vec3 colorSample  = color_transfer_function( maxSample );
	float alphaSample = alpha_transfer_function( maxSample );

	color.rgb = colorSample;
	color.a   = alphaSample;
}

void render_sampled_voxels( )
{
	if( true )
	{
		render_sampled_voxels_xray( );
	}
	else
	{
		render_sampled_voxels_high_intensity_projection( );
	}
}


void main( )
{
	if( u_render_pass == RENDER_PASS_BACK_VOXELS )
	{
		render_back_voxels( );
	}
	else /* RENDER_PASS_SAMPLED_VOXELS */
	{
		render_sampled_voxels( );
	}
}
