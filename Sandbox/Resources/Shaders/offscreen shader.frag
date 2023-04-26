#version 330

in vec2 Frag_Texcoords;

out vec4 OutColor;

uniform vec3 color;
uniform sampler2D myTex;

void main()
{				 
	vec4 col = texture(myTex, Frag_Texcoords);
	OutColor = col;
}