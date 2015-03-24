// MarchingCubes.vs
#version 410

// Position of current voxel's minimum vertex within parent Block.
// position.x in [0, gridWidth - 1]
// position.y in [0, gridHeight - 1]
layout (location = 0) in vec2 position;

uniform sampler3D volumeData;
uniform float gridWidth;
uniform float gridHeight;
uniform float gridDepth;
uniform float isoSurfaceValue;

out vsOutGsIn {
	vec3 wsPosition; // World-space position of voxel's minimum corner (v0).
	vec3 wsVoxelSize; // World-space voxel size in each dimension xyz.
	vec4 f0123; // Density values at all
	vec4 f4567; // ... 8 voxel corners.
	uint mc_case; // 0-255, triTable case.
	float isoValue; // iso-surface value.
} vs_out;

void main() {
	vec3 uvw;
	uvw.x = (position.x + 0.5) / gridWidth;
	uvw.y = (position.y + 0.5) / gridHeight;
	uvw.z = (gl_InstanceID + 0.5) / gridDepth;

	vs_out.wsPosition = vec3(0);

	vec4 step = vec4(1.0/gridWidth, 1.0/gridHeight, 1.0/gridDepth, 0.0);
	vs_out.wsVoxelSize = 2.0*step.xyz;

	// Sample 4 lower level voxel corners:
	vs_out.f0123 = vec4( texture(volumeData, uvw).r,
						 texture(volumeData, uvw + step.xww).r,
						 texture(volumeData, uvw + step.xyw).r,
						 texture(volumeData, uvw + step.wyw).r );

	// Sample 4 upper level voxel corners:
	vs_out.f4567 = vec4( texture(volumeData, uvw + step.wwz).r,
						 texture(volumeData, uvw + step.xwz).r,
						 texture(volumeData, uvw + step.xyz).r,
						 texture(volumeData, uvw + step.wyz).r );

	vs_out.isoValue = isoSurfaceValue;
	vec4 threshold = vec4(isoSurfaceValue);

	// Each vector element corresponds to a voxel corner.  An element is
	// set to 1 if it's corner is above the isoSurfaceValue.  An element
	// is set to 0 it's corner is below the isoSurfaceValue.
	uvec4 n0123 = uvec4(step(threshold, vs_out.f0123));
	uvec4 n4567 = uvec4(step(threshold, vs_out.f4567));

	// Construct the bit encoded mc_case:
	vs_out.mc_case = (n0123.x) | (n0123.y << 1) | (n0123.z << 2) | (n0123.w << 3) |
				   (n4567.x << 4) | (n4567.y << 5) | (n4567.z << 6) | (n4567.w << 7);
}
