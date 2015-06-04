// CopyTexture.fs
#version 410

in vec3 uvw;

uniform sampler3D sourceTexture;

out float outValue;

void main() {

	outValue = texture(sourceTexture, uvw).r;
}
