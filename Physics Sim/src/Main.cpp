#include "Header.h"
#include "ShaderBuilder.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

using namespace glm;

#define defaultPlaneSize	2.f
#define defaultPlaneHeight	2.f
#define clothPlaneSize		0.25f
#define clothPlaneHeight	0.5f

#define antiAliasing		4

const GLfloat clearColor[] = { 0.f, 0.f, 0.f };

int		state = singleSpringState;
bool	stateChange = false, 
		simulation = true;
float	planeHeight = defaultPlaneHeight,
		planeSize = defaultPlaneSize;

GLuint	springVertexArray, 
		massVertexArray, 
	
		planeProgram,
		springProgram, 
		massProgram;

std::vector<Mass> massVec;
std::vector<Spring> springVec;

// buffer generation
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


// system generation
int userInput(std::string str)
{
	// get user input, restricted to integers
	do
	{
		std::cout << str;
		std::string input;
		std::cin >> input;
		bool digit = true;
		for (unsigned int i = 0; i < input.length(); i++)
		{
			if (!isdigit(input[i]))
			{
				std::cout << "Need to enter an integer amount" << std::endl;
				digit = false;
				break;
			}
		}
		if (digit) return stoi(input);
	} while (true);
}

void generateSingleSpringSystem()
{
	//Masses
	Mass fixed;
	Mass weight;
	fixed.position = vec3(0.f, planeHeight, 0.f);
	fixed.fixed = true;

	weight.position = vec3(0.2f, planeHeight - 1.f, 0.f);

	massVec.push_back(fixed);
	massVec.push_back(weight);

	Spring spring;
	spring.m1 = 0;	// massVec index of fixed
	spring.m2 = 1;	// massVec index of weight
	spring.constant = 50;
	spring.restLength = 2;

	springVec.push_back(spring);
}

void generateMultiSpringSystem()
{
	int numOfMasses = userInput("Enter number of masses on spring chain: ");

	// always need the single fixed point at the top
	Mass fixed;
	fixed.position = vec3(0.f, planeHeight, 0.f);
	fixed.fixed = true;
	massVec.push_back(fixed);


	for (int i = 0; i < numOfMasses; i++)
	{
		Mass m;
		m.position = massVec[i].position - glm::vec3(0.f, 0.7f, 0.f);
		m.mass = (i + 1) / 2.f;
		massVec.push_back(m);

		Spring s;
		s.m1 = i;
		s.m2 = i + 1;
		springVec.push_back(s);
	}
}

void generateCubeSpringSystem()
{
	int		numOfLayers = userInput("Enter number of cube layers: "),
			top = numOfLayers / 2,
			bottom = -top,
			springLayers = 1;
	float	massDistance = .2f,
			// equation of a sphere. all masses contained need to be connected to center mass
			// + .01f for floating point error
			springDistance = .01f + sqrt(3.f * pow(springLayers * massDistance, 2.f));

	// generate the network of masses
	float x = bottom * massDistance;
	for (float xLayer = 0; xLayer < numOfLayers; xLayer++)
	{
		float y = bottom * massDistance;
		for (int yLayer = 0; yLayer < numOfLayers; yLayer++)
		{
			float z = bottom * massDistance;
			for (float zLayer = 0; zLayer < numOfLayers; zLayer++)
			{
				Mass m;
				m.position = glm::vec3(x, y, z);
				massVec.push_back(m);
				z += massDistance;
			}
			y += massDistance;
		}
		x += massDistance;
	}


	// generate the spring network

	for (unsigned int i = 0; i < massVec.size(); i++)
	{
		for (unsigned int j = i + 1; j < massVec.size(); j++)
		{
			float dist = distance(massVec[i].position, massVec[j].position);
			if (dist < springDistance)
			{
				Spring s;
				s.m1 = i;
				s.m2 = j;
				s.restLength = dist;
				s.constant = 2000.f;
				springVec.push_back(s);
			}
		}
	}
}

void generateClothHangSpringSystem()
{
	int numOfLayers = userInput("Enter diameter of cloth: "),
		springLayers = 2,
		top = numOfLayers / 2,
		bottom = -top;
	float	massDistance = .005f,
		massMass = .1f,
		// equation of a sphere. all masses contained need to be connected to center mass
		// + .01f for floating point error
		springDistance = .01f + sqrt(3.f * pow(springLayers * massDistance, 2.f)),
		springConstant = 2000.f;



	// generate the network of masses
	float x = bottom * massDistance;
	for (float xLayer = 0; xLayer < numOfLayers; xLayer++)
	{
		float	y = bottom * massDistance,
			z = bottom * massDistance;
		for (int yLayer = 0; yLayer < numOfLayers; yLayer++)
		{
			Mass m;
			m.position = glm::vec3(x, y, z);
			m.mass = massMass;
			if (xLayer >= 5 && xLayer < 10 && yLayer >= 5 && yLayer < 10)
				m.fixed = true;
			massVec.push_back(m);

			y += massDistance;
			z += massDistance;
		}
		x += massDistance;
	}


	// generate the spring network
	for (unsigned int i = 0; i < massVec.size(); i++)
	{
		for (unsigned int j = i + 1; j < massVec.size(); j++)
		{
			float dist = distance(massVec[i].position, massVec[j].position);
			if (dist < springDistance)
			{
				Spring s;
				s.m1 = i;
				s.m2 = j;
				s.restLength = dist;
				s.constant = springConstant;
				springVec.push_back(s);
			}
		}
	}
}

void generateClothTableSpringSystem()
{
	int numOfLayers = userInput("Enter diameter of cloth: "),
		springLayers = 2,
		top = numOfLayers / 2,
		bottom = -top;
	float	massDistance = .005f,
			massMass = .1f,
		// equation of a sphere. all masses contained need to be connected to center mass
		// + .01f for floating point error
			springDistance = .01f + sqrt(3.f * pow(springLayers * massDistance, 2.f)),
			springConstant = 2000.f;
			



	// generate the network of masses
	float x = bottom * massDistance;
	for (float xLayer = 0; xLayer < numOfLayers; xLayer++)
	{
		float	y = bottom * massDistance,
				z = bottom * massDistance;
		for (int yLayer = 0; yLayer < numOfLayers; yLayer++)
		{
			Mass m;
			m.position = glm::vec3(x, y, z);
			m.mass = massMass;
			massVec.push_back(m);

			y += massDistance;
			z += massDistance;
		}
		x += massDistance;
	}


	// generate the spring network
	for (unsigned int i = 0; i < massVec.size(); i++)
	{
		for (unsigned int j = i + 1; j < massVec.size(); j++)
		{
			float dist = distance(massVec[i].position, massVec[j].position);
			if (dist < springDistance)
			{
				Spring s;
				s.m1 = i;
				s.m2 = j;
				s.restLength = dist;
				s.constant = springConstant;
				springVec.push_back(s);
			}
		}
	}
}


// rendering
void generateShaders()
{
	planeProgram = generateProgram("shaders/plane.vert",
									"shaders/plane.geom",
									"shaders/plane.frag");
	springProgram = generateProgram("shaders/springs.vert",
									"shaders/springs.frag");
	massProgram = generateProgram("shaders/masses.vert",
									"shaders/masses.frag");
}

void renderPlane(GLuint program)
{
    glUseProgram(program);

    passBasicUniforms(program);
	glUniform1f(glGetUniformLocation(program, "height"), planeHeight);
	glUniform1f(glGetUniformLocation(program, "planeSize"), planeSize);


    glDrawArrays(GL_POINTS, 0, 1);
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

	glPointSize(10);
	glDrawArrays(GL_POINTS, 0, massVec.size());

	glBindVertexArray(0);
}



int main()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, antiAliasing);

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

	generateSingleSpringSystem();


    glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window))
	{
		generateMassBuffer();
		generateSpringBuffer();


		// the rendering
        glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, clearColor);

		renderPlane(planeProgram);
		renderSprings(springProgram);
		if (state <= boxSpringState)	// only render masses if the object is not a cloth
			renderMasses(massProgram);
		
        glDisable(GL_DEPTH_TEST);
		glfwSwapBuffers(window);
		glfwPollEvents();
		


		// if we request a new scene
		if (stateChange)
		{
			massVec.clear();
			springVec.clear();
			planeHeight = defaultPlaneHeight;
			planeHeight = defaultPlaneHeight;
			switch (state)
			{
				case (singleSpringState):
					generateSingleSpringSystem();
					planeHeight = abs(planeHeight);
					break;
				case(multiSpringState):
					generateMultiSpringSystem();
					planeHeight = abs(planeHeight);
					break;
				case(boxSpringState):
					generateCubeSpringSystem();
					planeHeight = -abs(planeHeight);
					break;
				case(clothHangState):
					planeSize = clothPlaneSize;
					planeHeight = clothPlaneHeight;
					generateClothHangSpringSystem();
					planeHeight = -abs(planeHeight);
					break;
				case(clothTableState):
					planeSize = clothPlaneSize;
					planeHeight = clothPlaneHeight;
					generateClothTableSpringSystem();
					planeHeight = -abs(planeHeight);
					break;
				default:
					generateSingleSpringSystem();
					break;
			}
			stateChange = false;
		}
		// run physics sim unless paused
		else if (simulation)
		{
			// already moving at 60 steps/second
			for (int i = 0; i < timeStep; i++)
				springSystem(massVec, springVec, planeHeight, planeSize);
		}
	}


	// Shutdow the program
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}