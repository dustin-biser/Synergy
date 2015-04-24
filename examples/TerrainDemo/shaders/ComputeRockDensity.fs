// ComputeRockDensity.fs
#version 410

in vec3 uvw;

// World-space position of parent Block's min vetex
uniform vec3 wsBlockMinVertPos;

uniform sampler3D noiseTexture;
uniform mat3 rotMat;

// Inverse density texture dimenions
uniform vec3 inv_densityTextureDimensions;

out float density;


// Converts texture coordinates to world space coordinates
vec3 textureToWorldSpace(vec3 texCoord) {
	return vec3(texCoord.x, texCoord.z, -texCoord.y);
}

// Maps texture coordinates to relative position within Block.
// The relative position at Block's minVertex is (0,0,0) and
// the relative position at Block's maxVertex is (1,1,1).
vec3 relativePosWithinBlock(vec3 texCoord) {
	vec3 scaledCoord = texCoord - 0.5*inv_densityTextureDimensions;
	scaledCoord *= 1.0 / (vec3(1.0) - vec3(inv_densityTextureDimensions));
	return textureToWorldSpace(scaledCoord);
}

void main() {
	vec3 wsPos = wsBlockMinVertPos + relativePosWithinBlock(uvw);

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


//	// TODO Dustin - This causes gap in origin voxel for Blocks of dim 33x33x33
//	{
//		vec3 wsPos = wsBlockMinVertPos + relativePosWithinBlock(uvw);
//
//			vec3 garbage = texture(noiseTexture, wsPos).rgb;
//			garbage = rotMat * garbage;
//
//		density = -wsPos.y + 0.5;
//		density += sin(wsPos.z*3.145*2.02)*0.06;
//	}


//	{
//		vec3 wsPos = wsBlockMinVertPos + relativePosWithinBlock(uvw);
//
//			vec3 garbage = texture(noiseTexture, wsPos).rgb;
//			garbage = rotMat * garbage;
//
//		density = -wsPos.y;
//
//		// Primatives written per voxel: 24, 36, 24
//		density += -wsPos.z * 1.0/3.0;
//	}

}
