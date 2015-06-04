#version 410

in vec3 textureCoord;

uniform samplerCube skybox_texture;

out vec4 frag_colour;

void main () {
  frag_colour = texture (skybox_texture, textureCoord);
}