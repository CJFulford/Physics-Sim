#version 430 core

layout(points) in;
layout(triangle_Strip, max_vertices = 6) out;

uniform mat4 modelview;
uniform mat4 projection;
uniform float height;

float planeSize = 2.f;

void main (void)
{
    gl_Position = projection * modelview * vec4(-planeSize, height, -planeSize, 1.f);
	EmitVertex();
    gl_Position = projection * modelview * vec4(-planeSize, height, planeSize, 1.f);
	EmitVertex();
    gl_Position = projection * modelview * vec4(planeSize, height, -planeSize, 1.f);
	EmitVertex();
	EndPrimitive();

	gl_Position = projection * modelview * vec4(-planeSize, height, planeSize, 1.f);
	EmitVertex();
    gl_Position = projection * modelview * vec4(planeSize, height, -planeSize, 1.f);
	EmitVertex();
    gl_Position = projection * modelview * vec4(planeSize, height, planeSize, 1.f);
	EmitVertex();
	EndPrimitive();
}
