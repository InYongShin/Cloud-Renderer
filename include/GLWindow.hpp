#ifndef Window_hpp
#define Window_hpp


#include "GLTools.hpp"
#include "Viewer.hpp"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include <vector>



namespace IY{

class Window {
public:
	std::string name = "";
	int width = -1, height = -1;
	glm::vec4 clearColor = glm::vec4(0,0,0,1);
	GLFWwindow* window = nullptr;


	virtual void render() {
		glfwMakeContextCurrent(window);
		glfwGetWindowSize(window, &width, &height);
	}


	Window(int width, int height, const char* name) : width(width), height(height), name(name) {};

};


class GLWindow : public Window {
public:
	std::vector<Viewer*> viewers;
	glm::vec2 mousePt = glm::vec2(0);

	void addViewer(Viewer* viewer) {
		printf("Add viewer: %d, (%d x %d)\n", viewer->id, viewer->width, viewer->height);
		viewers.push_back(viewer);
	}


	virtual void render() {
		Window::render();


		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, width, height);
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		int start_x = 0;
		int start_y = height - viewers[0]->height;
		for(int i=0; i<viewers.size(); ++i) {
			if(i>1) { 
				start_x += viewers[i-1]->width;
			}
			if(i>0) {
				start_y = height - (viewers[0]->height + viewers[i]->height);
			}
			

			// viewers[i].camera.viewport = glm::vec2(width, height);
			viewers[i]->camera.viewport = glm::vec2(viewers[i]->width, viewers[i]->height);

			glEnable(GL_SCISSOR_TEST);
			glScissor(start_x, start_y, viewers[i]->width, viewers[i]->height);
			glClearColor(viewers[i]->clearColor.r, viewers[i]->clearColor.g, viewers[i]->clearColor.b, viewers[i]->clearColor.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_SCISSOR_TEST);

			glViewport(start_x, start_y, viewers[i]->width, viewers[i]->height);
			viewers[i]->render();
		}

		glfwSwapBuffers(window);
	}

	void setCallbackFunc() {
		glfwSetWindowUserPointer(window, this);
		glfwSetMouseButtonCallback(window, s_buttonCallback);
		glfwSetScrollCallback(window, s_scrollCallback);
		glfwSetCursorPosCallback(window, s_cursorCallback);
		glfwSetKeyCallback(window, s_keyCallback);
	}
	
	GLWindow(int width, int height, const char* name) : Window(width, height, name) { }

	void cursorCallback( const glm::vec2& pt ) {

		if( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_1 ) ) {
			/*glm::vec3 d = viewers[0]->camera.position - viewers[0]->camera.center;
			float dist = glm::length(d);
			float theta = atan2f(d.x,d.z);
			float phi   = atan2f(d.y,glm::length(glm::vec2(d.x,d.z)));
			theta -= (pt.x-mousePt.x)/viewers[0]->camera.viewport.x*pi;
			phi   += (pt.y-mousePt.y)/viewers[0]->camera.viewport.y*pi;
			d = glm::rotate( theta, glm::vec3(0,1,0) )*glm::rotate(phi,glm::vec3(-1,0,0)) * glm::vec4(0,0,dist,0);
			viewers[0]->camera.position = viewers[0]->camera.center + d;*/
			
			glm::vec2 offset = pt - mousePt;

			viewers[0]->camera.yaw += offset.x;
			viewers[0]->camera.pitch -= offset.y;
			if (viewers[0]->camera.pitch > 89.0f) viewers[0]->camera.pitch = 89.0f;
			if (viewers[0]->camera.pitch < -89.0f) viewers[0]->camera.pitch = -89.0f;

			glm::vec3 direction;
			direction.x = cos(glm::radians(viewers[0]->camera.yaw)) * cos(glm::radians(viewers[0]->camera.pitch));
			direction.y = sin(glm::radians(viewers[0]->camera.pitch));
			direction.z = sin(glm::radians(viewers[0]->camera.yaw)) * cos(glm::radians(viewers[0]->camera.pitch));
			viewers[0]->camera.center = viewers[0]->camera.position + direction;
		}
		mousePt = pt;
	}
	void buttonCallback( int button, int action, int mods ) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		mousePt = glm::vec2(x,y);

		

		// if( action == GLFW_PRESS ) pushFunc( button );
		// else if( action == GLFW_RELEASE) pullFunc( button );
	}
	void scrollCallback( double x, double y ) {
		/*glm::vec3 d = viewers[0]->camera.position - viewers[0]->camera.center;
		float dist = glm::length(d);
		dist *= powf(0.99f,float(y));
		viewers[0]->camera.position = viewers[0]->camera.center + dist*normalize(d);*/

		viewers[0]->camera.fov *= powf(0.99f, float(y));
	}
	void keyCallback( int key, int scan, int action, int mods ) {
		if (action == GLFW_PRESS) std::cout << key << std::endl;
		if (key == 87) { // w
			glm::vec3 frontDir = glm::normalize(viewers[0]->camera.center - viewers[0]->camera.position);
			viewers[0]->camera.position += frontDir * glm::vec3(0.1);
			viewers[0]->camera.center += frontDir * glm::vec3(0.1);
		}
		else if (key == 83) { // s
			glm::vec3 frontDir = glm::normalize(viewers[0]->camera.center - viewers[0]->camera.position);
			viewers[0]->camera.position -= frontDir * glm::vec3(0.1);
			viewers[0]->camera.center -= frontDir * glm::vec3(0.1);
		}
		else if (key == 65) { // a
			glm::vec3 frontDir = glm::normalize(viewers[0]->camera.center - viewers[0]->camera.position);
			glm::vec3 up = glm::vec3(0,1,0);
			glm::vec3 leftDir = glm::cross(up, frontDir);
			viewers[0]->camera.position += leftDir * glm::vec3(0.1);
			viewers[0]->camera.center += leftDir * glm::vec3(0.1);
		}
		else if (key == 68) { // d
			glm::vec3 frontDir = glm::normalize(viewers[0]->camera.center - viewers[0]->camera.position);
			glm::vec3 up = glm::vec3(0,1,0);
			glm::vec3 rightDir = glm::cross(frontDir, up);
			viewers[0]->camera.position += rightDir * glm::vec3(0.1);
			viewers[0]->camera.center += rightDir * glm::vec3(0.1);
		}
		else if (key == 69) { // e
			viewers[0]->camera.position += glm::vec3(0,.1,0);
			viewers[0]->camera.center += glm::vec3(0,.1,0);
		}
		else if (key == 81) { // q
			viewers[0]->camera.position += glm::vec3(0,-.1,0);
			viewers[0]->camera.center += glm::vec3(0,-.1,0);
		}
	}
	static void s_cursorCallback(GLFWwindow* window, double x, double y) {
		GLWindow* win = (GLWindow*)glfwGetWindowUserPointer(window);
		win->cursorCallback(glm::vec2(x,y));
	}
	static void s_buttonCallback(GLFWwindow* window, int button, int action, int mods) {
		GLWindow* win = (GLWindow*)glfwGetWindowUserPointer(window);
		win->buttonCallback(button, action, mods);
	}
	static void s_scrollCallback(GLFWwindow* window, double x, double y) {
		GLWindow* win = (GLWindow*)glfwGetWindowUserPointer(window);
		win->scrollCallback(x,y);
	}
	static void s_keyCallback(GLFWwindow* window, int key, int scan, int action, int mods) {
		GLWindow* win = (GLWindow*)glfwGetWindowUserPointer(window);
		win->keyCallback(key, scan, action, mods);
	}

};


struct SlideF {
	int tapID = -1;
	std::string name = "";
	float* value = nullptr;
	std::vector<float> range;

	SlideF() {}
	SlideF(int tapID, const std::string& name, float* value, float min, float max, float step) : tapID(tapID), name(name), value(value) {
		range.push_back(min);
		range.push_back(max);
		range.push_back(step);
	}
};

struct SlideI {
	int tapID = -1;
	std::string name = "";
	int* value = nullptr;
	std::vector<int> range;

	SlideI() {}
	SlideI(int tapID, const std::string& name, int* value, int min, int max, int step) : tapID(tapID), name(name), value(value) {
		range.push_back(min);
		range.push_back(max);
		range.push_back(step);
	}
};


class UIWindow : public Window {
private:
	struct nk_glfw glfw = { 0 };
	struct nk_context* ctx = nullptr;

	int curTap = -1;
	std::vector<std::string> taps;

	std::vector<SlideF> slideFs;
	std::vector<SlideI> slideIs;

public:

	void addTap(const std::string& name) {
		taps.push_back(name);
		curTap = taps.size()-1;
	}

	void addSlideF(const std::string& name, float* value, float min, float max, float step) {

		slideFs.emplace_back(SlideF(curTap, name, value, min, max, step));

		// slideFs.emplace_back(std::make_pair(name,value));
		// std::vector<float> rangeVec{ min, max, step };
		// slideFsRange.emplace_back(rangeVec);
	}

	void addSlideI(const std::string& name, int* value, int min, int max, int step) {

		slideIs.emplace_back(SlideI(curTap, name, value, min, max, step));

		// slideIs.emplace_back(std::make_pair(name, value));
		// std::vector<int> rangeVec{ min, max, step };
		// slideIsRange.emplace_back(rangeVec);
	}

	void init() {
		ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_DEFAULT);
		struct nk_font_atlas* atlas;
		nk_glfw3_font_stash_begin(&glfw, &atlas);
		nk_glfw3_font_stash_end(&glfw);
	}

	virtual void render() {
		Window::render();

		nk_glfw3_new_frame(&glfw);

		if (nk_begin(ctx, "UI", nk_rect(0, 0, width, height),
					 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		{
			for (int tap=0; tap<taps.size(); ++tap) {
				std::string tapName = taps[tap];
				if (nk_tree_push(ctx, NK_TREE_TAB, tapName.c_str(), NK_MAXIMIZED))
				{
					for (int i=0; i < slideFs.size(); ++i) {
						if(slideFs[i].tapID == tap){
							nk_layout_row_dynamic(ctx, 25, 2);
							nk_labelf(ctx, NK_TEXT_LEFT, "%s: %.3f", slideFs[i].name.c_str(), *slideFs[i].value);
							nk_slider_float(ctx, slideFs[i].range[0], slideFs[i].value, slideFs[i].range[1], slideFs[i].range[2]);
						}
					}

					for (int i=0; i<slideIs.size(); ++i) {
						if(slideIs[i].tapID == tap){
							nk_layout_row_dynamic(ctx, 25, 2);
							nk_labelf(ctx, NK_TEXT_LEFT, "%s: %d", slideIs[i].name.c_str(), *slideIs[i].value);
							nk_slider_int(ctx, slideIs[i].range[0], slideIs[i].value, slideIs[i].range[1], slideIs[i].range[2]);
						}
					}

					nk_tree_pop(ctx);
				}
			}
		}
		nk_end(ctx);

		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

		glfwSwapBuffers(window);
	}


	UIWindow(int width, int height, const char* name) : Window(width, height, name) {};


};



}


#endif /* Window_hpp */

