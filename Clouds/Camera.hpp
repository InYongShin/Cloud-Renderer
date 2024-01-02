#ifndef Camera_hpp
#define Camera_hpp


#include "GLTools.hpp"


namespace IY{

class Camera {

public:
	glm::vec3 position = glm::vec3(0,0,5);
	glm::vec3 center = glm::vec3(1,0,4);
	glm::vec2 viewport = glm::vec2(0);
	float fov = 60 / 180.f * pi;
	float zNear = 0.1f;
	float zFar = 1000.f;
	float yaw = 0, roll = 0, pitch = 0;


	glm::mat4 projMat() const {
		return glm::perspective(fov, viewport.x / viewport.y, zNear, zFar);
	}
	glm::mat4 viewMat() const {
		return glm::lookAt(position, center, glm::vec3(0, 1, 0));
	}



};

}



#endif /* Camera_hpp */