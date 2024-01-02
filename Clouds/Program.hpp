#ifndef Program_hpp
#define Program_hpp


#include "StringTools.hpp"
#include "GLTools.hpp"


#include <fstream>


namespace IY {

class Program {
private:
	GLuint programID = 0;
	GLuint vertexID = 0;
	GLuint fragmentID = 0;
	GLuint geometryID = 0;
	GLuint tessControlID = 0;
	GLuint tessEvaluateID = 0;

	static void printInfoProgramLog(GLuint obj) {
		int infologLength = 0, charsWritten = 0;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength <= 0) return;
		char* infoLog = new char[infologLength];
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cerr << "Program Info:" << std::endl;
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}

	static inline void printInfoShaderLog(GLuint obj) {
		int infologLength = 0, charsWritten = 0;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength <= 0) return;
		char* infoLog = new char[infologLength];
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cerr << "Shader Info:" << std::endl;
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}

public:

	inline void loadShaders(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr,
							const char* tessControlPath = nullptr, const char* tessEvaluatePath = nullptr) {
		printf("Loading: ");

		programID = glCreateProgram();

		// Vertex Shader
		printf("%s, ", vertexPath);
		vertexID = glCreateShader(GL_VERTEX_SHADER);
		std::string vertexCode = loadText(vertexPath);
		const char* vCode = vertexCode.c_str();
		glShaderSource(vertexID, 1, &vCode, NULL);
		glCompileShader(vertexID);
		printInfoShaderLog(vertexID);
		glAttachShader(programID, vertexID);

		// Tessellation Control Shader
		if (tessControlPath != nullptr) {
			printf("%s, ", tessControlPath);
			tessControlID = glCreateShader(GL_TESS_CONTROL_SHADER);
			std::string tessControlCode = loadText(tessControlPath);
			const char* tcCode = tessControlCode.c_str();
			glShaderSource(tessControlID, 1, &tcCode, NULL);
			glCompileShader(tessControlID);
			printInfoShaderLog(tessControlID);
			glAttachShader(programID, tessControlID);
		}

		// Tessellation Evaluate Shader
		if (tessEvaluatePath != nullptr) {
			printf("%s, ", tessEvaluatePath);
			tessEvaluateID = glCreateShader(GL_TESS_EVALUATION_SHADER);
			std::string tessEvaluateCode = loadText(tessEvaluatePath);
			const char* teCode = tessEvaluateCode.c_str();
			glShaderSource(tessEvaluateID, 1, &teCode, NULL);
			glCompileShader(tessEvaluateID);
			printInfoShaderLog(tessEvaluateID);
			glAttachShader(programID, tessEvaluateID);
		}

		// Geometry Shader
		if (geometryPath != nullptr) {
			printf("%s, ", geometryPath);
			geometryID = glCreateShader(GL_GEOMETRY_SHADER);
			std::string geometryCode = loadText(geometryPath);
			const char* gCode = geometryCode.c_str();
			glShaderSource(geometryID, 1, &gCode, NULL);
			glCompileShader(geometryID);
			printInfoShaderLog(geometryID);
			glAttachShader(programID, geometryID);
		}

		// Fragment Shader
		printf("%s\n", fragmentPath);
		fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
		std::string fragmentCode = loadText(fragmentPath);
		const char* fCode = fragmentCode.c_str();
		glShaderSource(fragmentID, 1, &fCode, NULL);
		glCompileShader(fragmentID);
		printInfoShaderLog(fragmentID);
		glAttachShader(programID, fragmentID);


		glLinkProgram(programID);
		glUseProgram(programID);
		printInfoProgramLog(programID);
	}
	inline bool isUsable() const { return programID>0; }
	void use() {
		glUseProgram(programID);
		glErr("program use error");
	}
	void setUniform(const std::string& name, float v) const {
		glUniform1f(glGetUniformLocation(programID, name.c_str()), v);
	}
	void setUniform(const std::string& name, int v) const {
		glUniform1i(glGetUniformLocation(programID, name.c_str()), v);
	}
	void setUniform(const std::string& name, const glm::vec2& v) const {
		glUniform2f(glGetUniformLocation(programID, name.c_str()), v.x, v.y);
	}
	void setUniform(const std::string& name, const glm::vec3& v) const {
		glUniform3f(glGetUniformLocation(programID, name.c_str()), v.x, v.y, v.z);
	}
	void setUniform(const std::string& name, const glm::vec4& v) const {
		glUniform4f(glGetUniformLocation(programID, name.c_str()), v.x, v.y, v.z, v.w);
	}
	void setUniform(const std::string& name, const glm::mat3& v) const {
		glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, value_ptr(v));
	}
	void setUniform(const std::string& name, const glm::mat4& v) const {
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, value_ptr(v));
	}

	void setUniform(const std::string& name, const float* v, uint32_t count) const {
		glUniform1fv(glGetUniformLocation(programID, name.c_str()), count, v);
	}
	void setUniform(const std::string& name, const int* v, uint32_t count) const {
		glUniform1iv(glGetUniformLocation(programID, name.c_str()), count, v);
	}
	void setUniform(const std::string& name, const glm::vec2* v, uint32_t count) const {
		glUniform2fv(glGetUniformLocation(programID, name.c_str()), count, (float*)v);
	}
	void setUniform(const std::string& name, const glm::vec3* v, uint32_t count) const {
		glUniform3fv(glGetUniformLocation(programID, name.c_str()), count, (float*)v);
	}
	void setUniform(const std::string& name, const glm::vec4* v, uint32_t count) const {
		glUniform4fv(glGetUniformLocation(programID, name.c_str()), count, (float*)v);
	}
	void setUniform(const std::string& name, const glm::mat3* v, uint32_t count) const {
		glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), count, GL_FALSE, (float*)v);
	}
	void setUniform(const std::string& name, const glm::mat4* v, uint32_t count) const {
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), count, GL_FALSE, (float*)v);
	}

	void clear() {
		if (programID) glDeleteProgram(programID);
		if (vertexID) glDeleteShader(vertexID);
		if (fragmentID) glDeleteShader(fragmentID);
		if (geometryID) glDeleteShader(geometryID);
		if (tessControlID) glDeleteShader(tessControlID);
		if (tessEvaluateID) glDeleteShader(tessEvaluateID);
		programID = vertexID = fragmentID = geometryID = tessControlID = tessEvaluateID = 0;
	}
	virtual ~Program() {
		clear();
	}

};

}



#endif /* Program_hpp */
