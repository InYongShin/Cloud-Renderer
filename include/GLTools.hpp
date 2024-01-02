#ifndef GLTools_hpp
#define GLTools_hpp


#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <string>


namespace IY {

const float pi = 3.14159265359f;
	
	

inline void glErr(const std::string& message) {
	GLint err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%08X ", err);
		std::cerr << "GL Error: " << message << std::endl;
	}
}

}




#endif /* GLTools_hpp */


