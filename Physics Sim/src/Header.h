#pragma once

#include "Tools.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <vector>


#define PI 3.14159265359f
#define twoPI 2.f * PI
#define halfPI 0.5f * PI
#define gravity 9.81f

#define defaultZoom 3.f

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 500

const glm::mat4 identity(1.0f);

const glm::vec3 defaultUp(0.f, 1.f, 0.f),
                defaultCam(0.f, 0.5f, 2.f),
                defaultCenter(0.f, 5.f, 0.f);

struct Mass 
{
	Mass() : position(glm::vec3(0.f, 0.f, 0.f)), velocity(glm::vec3(0.f, 0.f, 0.f)), mass(1.f), fixed(false) { }
	glm::vec3 position;
	glm::vec3 velocity;
	float mass;
	bool fixed;
};

struct Spring
{
	Spring() :  restLength(0.f), constant(.5f) { }
	Mass m1, m2;
	float restLength;
	float constant;
};

void generateShaders();

void passBasicUniforms(GLuint program);
void errorCallback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_motion(GLFWwindow* window, double x, double y);
void printOpenGLVersion(GLenum majorVer, GLenum minorVer, GLenum langVer);