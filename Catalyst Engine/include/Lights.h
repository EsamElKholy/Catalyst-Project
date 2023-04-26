#pragma once

#include <cinder\CinderMath.h>
#include "Texture.h"

using namespace Cinder::Math;

struct DirectionalLight
{
	vec3f Direction;

	vec3f AmbientColor;
	vec3f DiffuseColor;
	vec3f SpecularColor;

	float AmbientStrength;
};

struct Attenuation
{
	float Constant;
	float Linear;
	float Quadratic;
};

struct PointLight
{
	vec3f Position;

	vec3f AmbientColor;
	vec3f DiffuseColor;
	vec3f SpecularColor;

	Attenuation Atten;

	float AmbientStrength;
};