#include "Header.h"
#include <omp.h>
#define dampening		1.f		// this is good with a default mass of 1
#define collisionBuffer	.01f	// to prevent clipping	

using namespace glm;

void springSystem(std::vector<Mass> &masses, std::vector<Spring> &springs, float planeHeight, float planeSize)
{
	// apply spring force to all masses
	#pragma omp parallel (dynamic)
	for (unsigned int i = 0; i < springs.size(); i++)
	{
		Spring s = springs[i];
		vec3	p1 = masses[s.m1].position,
				p2 = masses[s.m2].position;

		float length = distance(p1, p2);
		float magnitude = -s.constant * (length - s.restLength);
		vec3 direction = normalize(p1 - p2);

		vec3 force = magnitude * direction;

		masses[s.m1].force += force;
		masses[s.m2].force += -force;
	}

	// apply forces to masses
	#pragma omp parallel (dynamic)
	for (unsigned int i = 0; i < masses.size(); i++)
	{
		Mass *m = &masses[i];
		if (!m->fixed)
		{
			m->force.y -= gravity * m->mass;
			// dampen the force
			m->force += (-dampening * m->velocity);
			// convert mass to accelleration and apply to velocity for change in time
			m->velocity += m->force / m->mass * (1.f / stepsPerSecond);

			// collision with the plane
			// check collision height
			if (abs(m->position.y - planeHeight) < collisionBuffer)
				//check collision bounds
				if (m->position.x < planeSize + collisionBuffer &&
					m->position.x > -planeSize - collisionBuffer &&
					m->position.z < planeSize + collisionBuffer &&
					m->position.z > -planeSize - collisionBuffer)
						m->velocity.y = 0;

			m->position += m->velocity * (1.f / stepsPerSecond);
			m->force = vec3(0.f, 0.f, 0.f);
		}
	}
}