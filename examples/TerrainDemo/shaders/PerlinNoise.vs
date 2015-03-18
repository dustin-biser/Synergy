// PerlinNoise.vs
// For generating a viewport filling triangle without vbo data.
#version 410

out int instanceID;

void main() {
/*
    Map the following:
    gl_VertexID == 0 -> (-1,-1)
    gl_VertexID == 1 -> ( 3,-1)
    gl_VertexID == 2 -> (-1, 3)
*/
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);

    instanceID  = gl_InstanceID;

    gl_Position = vec4(x, y, 0, 1);
}

