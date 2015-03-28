// ComputeNormalAmbo.vs
#version 410

// 3D texture coordinates
in vec3 uvw;

uniform sampler3D densityGrid;

//-- Inverse dimensions for densityGrid texture:
uniform float inv_gridWidth;
uniform float inv_gridHeight;
uniform float inv_gridDepth;

// Surface normal and ambient occlusion factor.
// Pack data into vec4 as follows:
// RGB -> normal.xyz
// Alpha -> ambient occlusion factor
out vec4 normalAmbo;

void main() {

	vec4 step = vec4(inv_gridWidth, inv_gridHeight, inv_gridDepth, 0.0);

	vec3 gradient;
	gradient.x = texture(densityGrid, uvw + step.xww).r -
				 texture(densityGrid, uvw - step.xww).r;

	gradient.y = texture(densityGrid, uvw + step.wyw).r -
				 texture(densityGrid, uvw - step.wyw).r;

	gradient.z = texture(densityGrid, uvw + step.wwz).r -
				 texture(densityGrid, uvw - step.wwz).r;

	normalAmbo.rgb = normalize(-gradient);

	// TODO Dustin compute ambient occlusion factor and pack into alpha component
	// of normalAmbo output.
}

