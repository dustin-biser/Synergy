// BlockEdges.vs
#version 410

layout (location = 0) in vec3 v_position;

// Stored in the multiplication order (Proj * View)
uniform mat4 ViewProjMatrix;

// World-space position of parent block's minimum  vertex.
uniform vec3 wsBlockMinVertPos;

void main() {
	mat4 translate = mat4(1.0);
	translate[3] = vec4(wsBlockMinVertPos, 1.0);

    gl_Position = ViewProjMatrix * translate * vec4(v_position, 1.0);
}
