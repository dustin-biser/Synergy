// RenderTexture.fs
#version 410

in vec2 texCoord;

uniform sampler2D texture2D;

out vec4 fragColor;

void main () {
    fragColor = texture(texture2D, texCoord);
}