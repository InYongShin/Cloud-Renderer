#version 410 core

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
// layout(location=2) in vec2 inTexCoord;
layout(location=3) in vec3 inTex3Coord;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 modelMat = mat4(1);

out mat3 lookAt;
out vec3 worldPos;
out vec3 normal;
// out vec2 texCoord;
out vec3 texCoord;


void main(){
	vec4 world_Pos = modelMat * vec4(inPosition,1);
	worldPos = world_Pos.xyz;
	normal = normalize( (modelMat*vec4(inNormal,0)).xyz );
	// texCoord = inTexCoord;
	texCoord = inTex3Coord;
	lookAt = mat3(viewMat);
	gl_Position = projMat * viewMat * world_Pos;
}
