// PerlinNoise.gs
#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in int instanceID[];

flat out float layer;

void main() {
    gl_Layer = instanceID[0];
    layer = float(gl_Layer);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}