// ComputeRockDensity.fs
#version 410

// 3D texture coordinates
in vec3 uvw;

// World-space position of parent Block's min vetex
uniform vec3 wsBlockMinVertPos;

out float density;

// Converts texture coordinates to world space coordinates
vec3 textureToWorldSpace(vec3 texCoord) {
	return vec3(texCoord.x, texCoord.z, -texCoord.y);
}

void main() {
	vec3 wsPos = wsBlockMinVertPos + textureToWorldSpace(uvw);

	density = -wsPos.y + 0.5;
}
