#version 330 core

in vec4 vColor;
in vec2 vUV;

out vec4 FragColor;

uniform vec4 multiplyColor;
uniform sampler2D textureMain;
uniform vec2 tile;

void main() {
    FragColor = texture(textureMain, (vUV + tile) * 0.125) * vColor * multiplyColor;
}
