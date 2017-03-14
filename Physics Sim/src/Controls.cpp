#include "Header.h"

#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace glm;

double  mouse_old_x,
		mouse_old_y;

float   rotate_x = 0.0,
		rotate_y = 0.0,
		zoom = defaultZoom,
		aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

vec3	up(0.f, 1.f, 0.f),
		cam(0.f, 0.5f, 2.f),
		center(0.f, 0.f, 0.f);


void passBasicUniforms(GLuint program)
{
	mat4   modelview = lookAt(cam * zoom, center, up),
		projection = perspective(45.0f, aspectRatio, 0.01f, 100.0f);

	mat4   rotationX = rotate(identity, rotate_x  * PI / 180.0f, vec3(1.f, 0.f, 0.f)),
		rotationY = rotate(rotationX, rotate_y  * PI / 180.0f, vec3(0.f, 1.f, 0.f));

	modelview *= rotationY;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, value_ptr(modelview));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, value_ptr(projection));
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

void printOpenGLVersion(GLenum majorVer, GLenum minorVer, GLenum langVer)
{
	GLint major, minor;
	glGetIntegerv(majorVer, &major);
	glGetIntegerv(minorVer, &minor);
	printf("OpenGL on %s %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
	printf("GLSL version supported %s\n", glGetString(langVer));
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