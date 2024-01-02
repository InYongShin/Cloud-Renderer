#ifndef Simulator_hpp
#define Simulator_hpp


#include "GLWindow.hpp"


namespace IY{

class Simulator {
private:
	GLWindow* window = nullptr;
	UIWindow* UIwindow = nullptr;
	

public:
	

	void setTap(const std::string& name) {
		UIwindow->addTap(name);
	}

	void addSliderFUI(const std::string& name, float* value, float min, float max, float step) {
		UIwindow->addSlideF(name, value, min, max, step);
	}

	void addSliderIUI(const std::string& name, int* value, int min, int max, int step) {
		UIwindow->addSlideI(name, value, min, max, step);
	}

	void setViewer(Viewer* viewer) {
		window->addViewer(viewer);
	}


	void initGL(int width, int height, const char* name) {
		if (!glfwInit()) {
			fprintf(stderr, "Failed to setup GLFW\n");
			exit(EXIT_FAILURE);
		}
		window->window = glfwCreateWindow(width, height, name, NULL, NULL);
		UIwindow->window = glfwCreateWindow(400, 800, "UI", NULL, NULL);

		if (window->window == NULL || UIwindow->window == NULL) {
			fprintf(stderr, "Failed to create window");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(window->window);
		if (glewInit()) {
			fprintf(stderr, "Failed to setup GLEW\n");
			exit(EXIT_FAILURE);
		}
		
		glfwMakeContextCurrent(UIwindow->window);
		UIwindow->init();
	}

	

	void run() {
		glfwMakeContextCurrent(window->window);
		if (window->viewers.size() < 1) {
			printf("No viewer. Create new viewer (%d x %d).\n", window->width, window->height);
			window->viewers.emplace_back(new Viewer3D(1, window->width, window->height));
			// window->viewers.front().loadProgram("render.vert", "render.frag"); // default shader (TODO: not file. only text)
		}

		for (auto& viewer : window->viewers) {
			viewer->initProgram();
		}

		while (!glfwWindowShouldClose(window->window)) {
			
			window->render();
			UIwindow->render();

			glfwPollEvents();
		}
		glfwDestroyWindow(window->window);
		glfwDestroyWindow(UIwindow->window);
		glfwTerminate();
	}


	Simulator() {};
	Simulator(int width, int height, const char* name) {
		window = new GLWindow(width, height, name);
		UIwindow = new UIWindow(width, height, name);
		initGL(width, height, name);
		window->setCallbackFunc();
	};

};

}



#endif /* Simulator_hpp */


