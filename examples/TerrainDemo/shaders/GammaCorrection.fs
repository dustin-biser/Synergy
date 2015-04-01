// GammaCorrection.fs
#version 410

in vec2 texCoord;

uniform sampler2D texture2D;

uniform float inv_gamma;

out vec4 fragColor;

void main () {
    vec3 color = texture(texture2D, texCoord).rgb;

    fragColor = vec4( pow(color, vec3(inv_gamma)), 1.0);
}