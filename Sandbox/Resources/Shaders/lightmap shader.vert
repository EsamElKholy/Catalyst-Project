#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec2 frag_texcoord;
out vec3 frag_normal;
out vec3 frag_pos;
out vec3 frag_eyePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 eyePos;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	frag_texcoord = texcoord;
	frag_normal = (model * vec4(normal, 0)).xyz;
	frag_pos = (model * vec4(position, 1.0f)).xyz;
	frag_eyePos = eyePos;
}