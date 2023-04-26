#pragma once


#include "Transform.h"
#include "InputManager.h"

#include <cinder\CinderMath.h>

using namespace Cinder::Math;

class IntersectionTest
{
public:
	~IntersectionTest() {}

private:
	IntersectionTest() {}

public:
	static std::pair<float, bool> OBBTest(vec3f &origin, vec3f &direction, vec3f &minBounds, vec3f &maxBounds, Transform *trans, Transform *parent = NULL);
};
