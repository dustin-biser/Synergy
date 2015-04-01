// ComputeRockDensity.fs
#version 410

// 3D texture coordinates
in vec3 uvw;

// World-space position of parent Block's min vetex
uniform vec3 wsBlockMinVertPos;

uniform sampler3D noiseTexture;
uniform mat3 rotMat;

out float density;

// Converts texture coordinates to world space coordinates
vec3 textureToWorldSpace(vec3 texCoord) {
	return vec3(texCoord.x, texCoord.z, -texCoord.y);
}

void main() {
	vec3 wsPos = wsBlockMinVertPos + textureToWorldSpace(uvw);

	density = -wsPos.y + 0.5;

	density += sin(wsPos.x*3.145*3)*0.1;
	wsPos = rotMat * wsPos;
	density += sin(wsPos.z*3.145*6.01)*0.05;
	wsPos = rotMat * wsPos;
	density += sin(wsPos.x*3.145*12.9)*0.025;

	density += texture(noiseTexture, wsPos).r * 0.1;
	wsPos = rotMat * wsPos;
	density += texture(noiseTexture, wsPos*1.97).r * 0.05;
	wsPos = rotMat * wsPos;
	density += texture(noiseTexture, wsPos*3.92).r * 0.025;
	wsPos = rotMat * wsPos;
	density += texture(noiseTexture, wsPos*7.88).r * 0.0125;


	float flat_spot_height = 0.3;
	vec2 flat_spot_xz_coord = vec2(1,-1);
	float outer_radius = 0.8;
	float inner_radius = 0.5;

	float distance_from_flat_spot = distance(wsPos.xz, flat_spot_xz_coord);

	float ratio = (outer_radius - distance_from_flat_spot) /
		(outer_radius - inner_radius);

	float flatten_amount = clamp(ratio, 0.0, 1.0) * 0.9;

	density = mix(density, (-wsPos.y + flat_spot_height), flatten_amount);


	// TODO Dustin - Use the following to display holes in terrain due to marching cubes
	// Holes seem to show up when sampling sin() in the z direction.
//	{
//		vec3 wsPos = wsBlockMinVertPos + textureToWorldSpace(uvw);
//
//			vec3 garbage = texture(noiseTexture, wsPos).rgb;
//			garbage = rotMat * garbage;
//
//		density = -wsPos.y + 0.5;
//		density += sin(wsPos.z*3.145*6.01)*0.05; // Here's where holes show up
//	}

}
