#version 410 core

out vec4 outColor;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;

uniform vec4 color;

void main(){
	outColor = color;
}
