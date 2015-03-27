// MarchingCubes.vs
#version 410

/*
* Each instance is a layer of GL_POINTs, where each point corresonds
* to a voxel within the layer.
*/

// Voxel 2D index position within layer instance of parent Block.
// position.x in [0, gridWidth - 1]
// position.y in [0, gridHeight - 1]
layout (location = 0) in vec2 position;

uniform sampler3D densityGrid;
uniform float inv_gridWidth;
uniform float inv_gridHeight;
uniform float inv_gridDepth;
uniform float isoSurfaceValue;

out vsOutGsIn {
	vec3 voxelIndex; // Voxel's index within parent Block.
	vec4 f0123; // Density values at all
	vec4 f4567; // ... 8 voxel corners.
	uint mc_case; // 0-255, triTable case.
	float isoValue; // iso-surface value.
} vs_out;

void main() {
	int zLayer = gl_InstanceID;
	vs_out.voxelIndex = vec3(position, zLayer);

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

	// Sample 4 upper level voxel corners:
	vs_out.f4567 = vec4( texture(densityGrid, uvw + step.wwz).r,
						 texture(densityGrid, uvw + step.xwz).r,
						 texture(densityGrid, uvw + step.xyz).r,
						 texture(densityGrid, uvw + step.wyz).r );

	vs_out.isoValue = isoSurfaceValue;

	// Each vector element corresponds to a voxel corner.  An element is
	// set to 1 if it's corner is above isoSurfaceValue.  An element
	// is set to 0 it's corner is below isoSurfaceValue.
	uvec4 n0123 = uvec4(step(isoSurfaceValue, vs_out.f0123));
	uvec4 n4567 = uvec4(step(isoSurfaceValue, vs_out.f4567));

	// Construct the bit encoded mc_case:
	vs_out.mc_case = (n0123.x) | (n0123.y << 1) | (n0123.z << 2) | (n0123.w << 3) |
				   (n4567.x << 4) | (n4567.y << 5) | (n4567.z << 6) | (n4567.w << 7);
}
