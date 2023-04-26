#version 330

in vec3 frag_pos;
out vec4 OutColor;

uniform vec3 color;
uniform vec3 eyePos;
uniform vec3 fogColor;
uniform float start;
uniform float end;
uniform float fogP;

void main()
{
	float dist = length(eyePos - frag_pos);

	//float fog = (end - dist) / (end - start);
	float fog = 1.0f / exp(dist * fogP * dist * fogP);
	fog = clamp(fog, 0, 1);
	vec3 col = mix(fogColor, color, fog);

	OutColor = vec4(col, 1.0f);
}