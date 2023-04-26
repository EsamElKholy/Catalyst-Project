#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

out vec2 Frag_Texcoords;

void main()
{
	gl_Position = vec4(position, 1.0f);
	Frag_Texcoords = texcoords;
}