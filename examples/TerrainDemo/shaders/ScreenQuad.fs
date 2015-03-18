// RenderTexture.fs
#version 410

flat in int instanceID;
in vec2 texCoord;

uniform sampler3D texture3D;
uniform int layer;

out vec4 fragColor;

void main () {
	vec3 texCoord3D = vec3(texCoord, float(layer) + 0.5);
    fragColor = texture(texture3D, texCoord3D);
}