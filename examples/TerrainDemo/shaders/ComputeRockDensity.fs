// ComputeRockDensity.fs
#version 410

// 3D texture coordinates
in vec3 uvw;

// World-space position of parent Block's min vetex
uniform vec3 wsBlockMinVertPos;

uniform sampler3D noiseTexture;

out float density;

// Converts texture coordinates to world space coordinates
vec3 textureToWorldSpace(vec3 texCoord) {
	return vec3(texCoord.x, texCoord.z, -texCoord.y);
}

void main() {
	vec3 wsPos = wsBlockMinVertPos + textureToWorldSpace(uvw);

	density = -wsPos.y + 0.5;
	density += texture(noiseTexture, wsPos).r * 0.04;
	density += texture(noiseTexture, wsPos*1.97).r * 0.02;
	density += texture(noiseTexture, wsPos*2.86).r * 0.01;

}
