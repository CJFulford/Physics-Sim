#pragma once

#include "Tools.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <vector>


#define PI					3.14159265359f
#define twoPI				2.f * PI
#define halfPI				0.5f * PI
#define gravity				9.81f

#define singleSpringState	0
#define multiSpringState	1
#define boxSpringState		2
#define clothHangState		3
#define clothTableState		4

#define WINDOW_WIDTH		700
#define WINDOW_HEIGHT		500

#define timeStep			10.f				// if timestep = 1, 60 steps/second
#define stepsPerSecond		(timeStep * 60.f)	// 20 = 1200 steps/second

#define identity		mat4(1.f)
#define defaultZoom		2.f
#define defaultCamUp	vec3(0.f, 1.f, 0.f)
#define defaultCamLoc	vec3(0.f, .5f, 2.f)
#define defaultCamCent	vec3(0.f, 0.f, 0.f)

extern int state;
extern bool stateChange, simulation;

struct Mass 
{
	Mass() :	position(glm::vec3(0.f, 0.f, 0.f)), 
				velocity(glm::vec3(0.f, 0.f, 0.f)),
				force(glm::vec3(0.f, 0.f, 0.f)),
				mass(1.f), 
				fixed(false) { }
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;
	float mass;
	bool fixed;
};

struct Spring
{
	Spring() :  restLength(.4f), 
				constant(50.f) { }
	unsigned int m1, m2;
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

void springSystem(std::vector<Mass> &masses, std::vector<Spring> &springs, float planeHeight, float planeSize);