// ViewportFillingTriangle.gs
#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in int instanceID[];
in vec2 texCoord[];

out vec3 uvw;

uniform float textureDepth;

void main() {
	// Geometry shader will ues gl_Layer as the layer to render into for
	// 3-dimensional framebuffer color attachments.
    gl_Layer = instanceID[0];

	// Compute texture coordinates uvw for each vertex.
	// Texel values lie at the center of texture grid cells.
	// The uvw coordinate for texel(i,j,k) can be computed as follows:
	// u = (i + 0.5) / textureWidth
	// v = (j + 0.5) / textureHeight
	// w = (k + 0.5) / textureDepth
	uvw = vec3(texCoord[0], (gl_Layer + 0.5) / textureDepth);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

	uvw = vec3(texCoord[1], (gl_Layer + 0.5) / textureDepth);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

	uvw = vec3(texCoord[2], (gl_Layer + 0.5) / textureDepth);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}