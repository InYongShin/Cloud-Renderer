#ifndef Viewport_hpp
#define Viewport_hpp


#include "GLTools.hpp"
#include "Program.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "TriMesh.hpp"

#include <vector>


namespace IY{

class Viewer {
public:
	int id = -1;
	int width = -1, height = -1;
	glm::vec4 clearColor = glm::vec4(.31, .73, .87, 1);
	Camera camera;

	virtual void initProgram() {

	}

	virtual void render() {

	}

	Viewer(int id, int width, int height) : id(id), width(width), height(height) {}
	virtual ~Viewer() {}

};


class Viewer3D : public Viewer {
public:
	Light light;

	TriMesh quad;
	TriMesh cube;
	TriMesh plane;
	TriMesh lightMesh;

	Program mainProg;
	Program planeProg;
	Program lightProg;

	float volumeScaleX=1.f;
	float volumeScaleY=1.f;
	float volumeScaleZ=1.f;

	float r1 = 1.0f;
	float r2 = 2.0f;

	int octave = 8;
	float rayStep = 0.1f;
	int maxStep = 128;
	float lightStep = 0.1f;
	int maxLightStep = 6;
	float frequency = 1.f;
	float H = -0.85f;
	float coverage = 0.2f;
	float absorption = 1.f;
	float lightAbsorptionToSun = 1.f;
	float g = 0.2f;

	virtual void initProgram() {
		if (!mainProg.isUsable()) {
			// Default Main shader (temp)
			mainProg.loadShaders("../Shaders/render2D.vert", "../Shaders/render2D.frag");
			planeProg.loadShaders("../Shaders/plane.vert", "../Shaders/plane.frag");
			lightProg.loadShaders("../Shaders/light.vert", "../Shaders/light.frag");
		}
	}

	// TODO: 바깥(main)에서 render function을 수정할 수 있게 만들어야 함.
	//       function pointer로 등록.
	virtual void render() {
		// TODO

		planeProg.use();
		planeProg.setUniform("modelMat", glm::mat4(1));
		planeProg.setUniform("viewMat", camera.viewMat());
		planeProg.setUniform("projMat", camera.projMat());
		planeProg.setUniform("backColor", glm::vec4(.7, .5, .3, 1.));
		// plane.renderQuad(planeProg, glm::rotate(pi/2.f, glm::vec3(1,0,0)) * glm::translate(glm::vec3(0,5,0)) * glm::scale(glm::vec3(1,1,1)));
		plane.renderPlane(planeProg, glm::translate(glm::vec3(0, -1, 0)) * glm::scale(glm::vec3(5, 1, 5)));

		lightProg.use();
		lightProg.setUniform("viewMat", camera.viewMat());
		lightProg.setUniform("projMat", camera.projMat());
		lightProg.setUniform("color", glm::vec4(1.));
		lightMesh.renderSphere(lightProg, 0.1f, glm::translate(light.position));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mainProg.use();
		// glErr("GL error");

		// static glm::vec3 tempPos = glm::vec3(0,5,0);
		// tempPos += glm::vec3(0,0,0.0001f);

		mainProg.setUniform("viewport", camera.viewport);
		// mainProg.setUniform("camPos", tempPos);
		mainProg.setUniform("camPos", camera.position);
		mainProg.setUniform("lookAt", camera.viewMat());
		mainProg.setUniform("lightPos", light.position);
		mainProg.setUniform("lightCol", light.color);
		// mainProg.setUniform("camPos", camera.position);
		mainProg.setUniform("camDir", glm::normalize(camera.center - camera.position));
		mainProg.setUniform("boundsMin", -glm::vec3(volumeScaleX, volumeScaleY-5, volumeScaleZ));
		mainProg.setUniform("boundsMax", glm::vec3(volumeScaleX, volumeScaleY+5, volumeScaleZ));
		mainProg.setUniform("r1", r1);
		mainProg.setUniform("r2", r2);

		mainProg.setUniform("rayStep", rayStep);
		mainProg.setUniform("lightStep", lightStep);
		mainProg.setUniform("maxLightStep", maxLightStep);
		mainProg.setUniform("frequency", frequency);
		mainProg.setUniform("numOctaves", octave);
		mainProg.setUniform("coverage", coverage);
		mainProg.setUniform("absorption", absorption);
		mainProg.setUniform("lightAbsorptionToSun", lightAbsorptionToSun);
		mainProg.setUniform("H", H);
		mainProg.setUniform("g", g);
		mainProg.setUniform("maxStep", maxStep);
		mainProg.setUniform("backColor", glm::vec4(.31, .73, .87, 1));
		quad.renderQuad(mainProg);
		// cube.renderCube(mainProg, glm::translate(glm::vec3(0,5,0)) * glm::scale(glm::vec3(volumeScaleX,volumeScaleY,volumeScaleZ)) );
		

		glDisable(GL_BLEND);
		glErr("Viewer3D Rendering");
	}


	Viewer3D(int id, int width, int height) : Viewer(id, width, height) {}
	virtual ~Viewer3D() {}
};


class Viewer2D : public Viewer {
public:

	Program quadProg;
	TriMesh quad;

	virtual void initProgram() {
		if (!quadProg.isUsable()) {
			// Default Quad shader (temp)
			quadProg.loadShaders("../Shaders/quad.vert", "../Shaders/quad.frag");
		}
	}

	virtual void render() {
		quadProg.use();

		quadProg.setUniform("backColor", glm::vec4(1));
		quad.renderQuad(quadProg);

		glErr("Viewer2D Rendering");
	}


	Viewer2D(int id, int width, int height) : Viewer(id, width, height) {}
	virtual ~Viewer2D() {}
};


}


#endif /* Viewport_hpp */