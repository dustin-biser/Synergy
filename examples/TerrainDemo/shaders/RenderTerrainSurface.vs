// RenderIsoSurface.vs
#version 410

layout (location = 0) in vec3 position; // vertex position.
layout (location = 1) in vec3 normal; // vertex normal.

uniform mat4 MVP_Matrix; // model-view-projection matrix.
uniform mat4 NormalMatrix; // transforms world-space normal to eye-space.

out v2fOutput {
	vec3 normal;
	vec3 wsNormal;
	vec3 dirToLight;
} vs_out;

void main() {
	gl_Position = MVP_Matrix * vec4(position, 1.0);

	vec3 dirToLight = vec3(0.5, 0.8, 1.0);
	vs_out.dirToLight = normalize( (NormalMatrix * vec4(dirToLight, 0.0)).xyz );

	vs_out.normal = (NormalMatrix * vec4(normal, 1.0)).xyz;
	vs_out.wsNormal = normal;
}
