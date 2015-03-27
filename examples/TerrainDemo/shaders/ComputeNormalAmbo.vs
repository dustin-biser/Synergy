// ComputeLighting.vs
#version 410

/*
* Each instance is a layer of GL_POINTs, where each point corresonds
* to a voxel within a grid layer.
*/

// Voxel 2D index position within layer instance of parent Block.
// position.x in [0, gridWidth - 1]
// position.y in [0, gridHeight - 1]
layout (location = 0) in vec2 position;

uniform float inv_gridWidth;
uniform float inv_gridHeight;
uniform float inv_gridDepth;

void main() {
	int zLayer = gl_InstanceID;

	vec3 uvw;
	uvw.x = (position.x + 0.5) * inv_gridWidth;
	uvw.y = (position.y + 0.5) * inv_gridHeight;
	uvw.z = (zLayer + 0.5) * inv_gridDepth;

	vec4 step = vec4(inv_gridWidth, inv_gridHeight, inv_gridDepth, 0.0);

	// Sample 4 lower level voxel corners:
	vs_out.f0123 = vec4( texture(densityGrid, uvw).r,
						 texture(densityGrid, uvw + step.xww).r,
						 texture(densityGrid, uvw + step.xyw).r,
						 texture(densityGrid, uvw + step.wyw).r );
}
