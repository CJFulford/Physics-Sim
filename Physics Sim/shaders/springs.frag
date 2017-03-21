#version 430 core

out vec4 color;

in vec3 vert;

void main (void)
{
	color = vec4(abs(vert), 0.f);
}
