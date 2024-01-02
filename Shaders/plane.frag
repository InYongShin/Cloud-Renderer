#version 410 core

out vec4 outColor;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;

uniform vec4 backColor;

void main(){
	outColor = backColor;
	// outColor = vec4(0.31, .73, .87, 1);
}
