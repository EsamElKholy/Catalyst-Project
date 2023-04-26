#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float increament;

void main()
{
	vec3 pos = position;
	pos.y = sin(pos.x + increament)*100.0f;
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	//gl_Position = projection * view * model * vec4(position, 1.0f);
}