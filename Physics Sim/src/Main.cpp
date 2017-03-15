#include "Header.h"
#include "ShaderBuilder.h"
#include <iostream>

using namespace glm;

const GLfloat clearColor[] = { 0.f, 0.f, 0.f };

int state = 0;
bool stateChange = false;

GLuint	ceilingVertexArray, 
		springVertexArray, 
		massVertexArray, 
	
		ceilingProgram, 
		springProgram, 
		massProgram;

std::vector<Mass> massVec;
std::vector<Spring> springVec;

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

void generateMassBuffer()
{

	GLuint massVertexBuffer = 0;

	glGenVertexArrays(1, &massVertexArray);
	glBindVertexArray(massVertexArray);

	std::vector<vec3> masses;

	for (unsigned int i = 0; i < massVec.size(); i++)
		masses.push_back(massVec[i].position);

	glGenBuffers(1, &massVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, massVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(masses[0]) * masses.size(), &masses[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
}



void generateSpringBuffer()
{
	GLuint springVertexBuffer = 0;

	glGenVertexArrays(1, &springVertexArray);
	glBindVertexArray(springVertexArray);

	std::vector<vec3> springs;

	for (unsigned int i = 0; i < springVec.size(); i++)
	{
		springs.push_back(massVec[springVec[i].m1].position);
		springs.push_back(massVec[springVec[i].m2].position);
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
	Mass fixed;
	Mass weight;
	fixed.position = vec3(0.f, 3.f, 0.f);
	fixed.fixed = true;

	weight.position = vec3(0.f, 2.2f, 0.f);

	massVec.push_back(fixed);
	massVec.push_back(weight);

	Spring spring;
	spring.m1 = 0;	// massVec index of fixed
	spring.m2 = 1;	// massVec index of weight
	spring.constant = 50;
	spring.restLength = 2;

	springVec.push_back(spring);
}

void generateMultiSpringSystem(int numOfMasses)
{
	// always need the single fixed point at the top
	Mass fixed;
	fixed.position = vec3(0.f, 3.f, 0.f);
	fixed.fixed = true;
	massVec.push_back(fixed);


	for (unsigned int i = 0; i < numOfMasses; i++)
	{
		Mass m;
		m.position = massVec[i].position - glm::vec3(0.f, 0.7f, 0.f);
		m.mass = 2.f * (i + 1);
		massVec.push_back(m);

		Spring s;
		s.m1 = i;
		s.m2 = i + 1;
		springVec.push_back(s);
	}
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
	glfwWindowHint(GLFW_SAMPLES, 16);

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
	printOpenGLVersion(GL_MAJOR_VERSION, GL_MINOR_VERSION, GL_SHADING_LANGUAGE_VERSION);

    generateShaders();
    generateCeilingBuffer();

	generateSingleSpringSystem();

	generateMassBuffer();
	generateSpringBuffer();

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
		
		if (stateChange)
		{
			massVec.clear();
			springVec.clear();
			switch (state)
			{
				case (0):
					generateSingleSpringSystem();
					break;
				case(1):
					generateMultiSpringSystem(2);
					break;
				default:
					generateSingleSpringSystem();
					break;
			}
			stateChange = false;
		}
		else
		{
			// already moving at 60 steps/second
			for (int i = 0; i < timeStep; i++)
				springSystem(massVec, springVec);
		}


		generateMassBuffer();
		generateSpringBuffer();
	}


	// Shutdow the program
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}