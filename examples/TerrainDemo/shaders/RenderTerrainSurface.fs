// RenderIsoSurface.fs
#version 410

in v2fOutput {
	vec3 normal;
	vec3 wsNormal;
	vec3 dirToLight;
} fs_in;

out vec4 out_color;

void main() {
	vec3 ambient = vec3(0.1, 0.1, 0.1);
	vec3 Kd = vec3(0.4, 0.4, 0.8); // Blue-ish material color.

	vec3 diffuse = Kd * max(dot(fs_in.dirToLight, fs_in.normal), 0.0);

	out_color.rgb = ambient + diffuse;
	out_color.a = 0.0;

	// TODO Dustin - Use to this visualize surface normals:
//	out_color.rgb = fs_in.wsNormal;
}

