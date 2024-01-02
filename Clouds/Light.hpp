#ifndef Light_hpp
#define Light_hpp


#include "GLTools.hpp"


namespace IY {

class Light {
public:
	glm::vec3 position = glm::vec3(1,1,1);
	glm::vec3 color = glm::vec3(1,1,1);


};

}




#endif /* Light_hpp */
