#include "Header.h"
#include "ShaderBuilder.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>

using namespace glm;


double  mouse_old_x, 
        mouse_old_y;
float   rotate_x = 0.0,
        rotate_y = 0.0,
        zoom = defaultZoom,
        aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

const GLfloat clearColor[] = { 0.f, 0.f, 0.f };

vec3	up(0.f, 1.f, 0.f),
		cam(0.f, 0.5f, 2.f),
		center(0.f, 0.f, 0.f);

GLuint	ceilingVertexArray, 
		springVertexArray, 
		massVertexArray, 
	
		ceilingProgram, 
		springProgram, 
		massProgram;

std::vector<Mass> massVec;
std::vector<Spring> springVec;

void errorCallback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_motion(GLFWwindow* window, double x, double y);
void printOpenGLVersion();

void generateCeilingBuffer()
{
    GLuint ceilingVertexBuffer = 0;

    glGenVertexArrays(1, &ceilingVertexArray);
    glBindVertexArray(ceilingVertexArray);

    std::vector<vec3> verts;

    verts.push_back(vec3(-2.f, 3.f, -2.f));
    verts.push_back(vec3(-2.f, 3.f, 2.f));
    verts.push_back(vec3(2.f, 3.f, -2.f));
    verts.push_back(vec3(2.f, 3.f, 2.f));

    glGenBuffers(1, &ceilingVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, ceilingVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts[0]) * verts.size(), &verts[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void generateMassSpringBuffer()
{
	GLuint massVertexBuffer = 0;

	glGenVertexArrays(1, &massVertexArray);
	glBindVertexArray(massVertexArray);

	std::vector<vec3> masses;

	for (int i = 0; i < massVec.size(); i++)
		masses.push_back(massVec[i].position);

	glGenBuffers(1, &massVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, massVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(masses[0]) * masses.size(), &masses[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	
	
	
	
	GLuint springVertexBuffer = 0;

	glGenVertexArrays(1, &springVertexArray);
	glBindVertexArray(springVertexArray);

	std::vector<vec3> springs;

	for (int i = 0; i < springVec.size(); i++)
	{
		springs.push_back(springVec[i].m1.position);
		springs.push_back(springVec[i].m2.position);
	}

	glGenBuffers(1, &springVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, springVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(springs[0]) * springs.size(), &springs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
}

void generateSingleSpringSystem()
{
	//Masses
	Mass fixed, weight;
	fixed.position = vec3(0.f, 3.f, 0.f);
	fixed.fixed = true;

	massVec.push_back(fixed);
	massVec.push_back(weight);
	

	// Springs
	Spring spring;
	spring.m1 = fixed;
	spring.m2 = weight;

	springVec.push_back(spring);
}

void generateShaders()
{
	ceilingProgram = generateProgram(	"shaders/general.vert",
										"shaders/general.frag");
	springProgram = generateProgram(	"shaders/general.vert",
										"shaders/springs.frag");
	massProgram = generateProgram(		"shaders/general.vert",
										"shaders/masses.frag");
}

void passBasicUniforms(GLuint program)
{
    mat4   modelview = lookAt(cam * zoom, center, up),
        projection = perspective(45.0f, aspectRatio, 0.01f, 100.0f);

    mat4   rotationX = rotate(identity , rotate_x  * PI / 180.0f, vec3(1.f, 0.f, 0.f)),
                rotationY = rotate(rotationX, rotate_y  * PI / 180.0f, vec3(0.f, 1.f, 0.f));

    modelview *= rotationY;

    glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, value_ptr(modelview));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void renderCeiling(GLuint program)
{
    glBindVertexArray(ceilingVertexArray);
    glUseProgram(program);

    passBasicUniforms(program);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
}

void renderSprings(GLuint program)
{
	glBindVertexArray(springVertexArray);
	glUseProgram(program);

	passBasicUniforms(program);
	
	glLineWidth(2);
	glDrawArrays(GL_LINES, 0, 2 * springVec.size());

	glBindVertexArray(0);
}

void renderMasses(GLuint program)
{
	glBindVertexArray(massVertexArray);
	glUseProgram(program);

	passBasicUniforms(program);

	glPointSize(15);
	glDrawArrays(GL_POINTS, 0, massVec.size());

	glBindVertexArray(0);
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, 32);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Physics Sim", NULL, NULL);

	if (!window) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_motion);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}
	printOpenGLVersion();

    generateShaders();
    generateCeilingBuffer();

	generateSingleSpringSystem();
	generateMassSpringBuffer();

    glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window))
	{
        glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, clearColor);


        renderCeiling(ceilingProgram);
		renderSprings(springProgram);
		renderMasses(massProgram);
		

        glDisable(GL_DEPTH_TEST);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Shutdow the program
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case(GLFW_KEY_N):
			std::cout << "Recompiling Shaders... ";
            generateShaders();
            std::cout << "Done" << std::endl;
			break;
		default:
			break;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset < 0)
        zoom += 0.1f;
    else if (yoffset > 0)
        zoom -= 0.1f;
}

void mouse_motion(GLFWwindow* window, double x, double y)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        rotate_x += (float)((y - mouse_old_y) * 0.5f);
        rotate_y += (float)((x - mouse_old_x) * 0.5f);
    }
    mouse_old_x = x;
    mouse_old_y = y;
}

void printOpenGLVersion()
{
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("OpenGL on %s %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
	printf("GLSL version supported %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("GL version major, minor: %i.%i\n", major, minor);
}

void errorCallback(int error, const char* description)
{
	std::cout << "GLFW ERROR " << error << ": " << description << std::endl;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}