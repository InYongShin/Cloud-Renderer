#version 410 core

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 modelMat = mat4(1);

out vec3 worldPos;
out vec3 normal;
out vec2 texCoord;

void main(){
	vec4 world_Pos = modelMat * vec4(inPosition,1);
	worldPos = world_Pos.xyz;
	normal = normalize( (modelMat*vec4(inNormal,0)).xyz );
	texCoord = inTexCoord;
	gl_Position = projMat * viewMat * modelMat * world_Pos;
}
