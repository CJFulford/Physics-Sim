#pragma once

#include "Tools.h"

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