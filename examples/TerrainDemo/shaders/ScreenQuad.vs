// ScreenQuad.vs
// For generating a viewport filling triangle without vbo data.
/*
    Maps the following:
    if (gl_VertexID == 0) -> (x,y) = (-1,-1)
    if (gl_VertexID == 1) -> (x,y) = ( 3,-1)
    if (gl_VertexID == 2) -> (x,y) = (-1, 3)

    After clipping, a viewport filling quad will be formed with
    bottom left corner at (-1,-1), and side length 2.
*/

#version 410

out vec2 texCoord;

void main() {
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);

    texCoord.x = (x + 1.0) * 0.5;
    texCoord.y = (y + 1.0) * 0.5;

    gl_Position = vec4(x, y, 0, 1);
}


