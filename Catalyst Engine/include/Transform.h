#pragma once

#include <cinder\CinderMath.h>

using namespace Cinder::Math;

class Transform
{
public:
	Transform(const vec3f &pos = vec3f(), const quatf &rot = quatf(), const vec3f &scale = vec3f(1, 1, 1))
		: Position(pos), Rotation(rot), Scale(scale)
	{
		OldPosition = vec3f();
		OldRotation = quatf();
		OldScale = vec3f(1, 1, 1);
		ParentMatrix = mat4f();
		InitializeOldStuff = false;
	}

	~Transform() {}

public:
	mat4f GetModelMatrix();
	void Rotate(float angle, vec3f &axis);
	void Translate(float amount, vec3f &axis);
	void LookAt(vec3f &point, vec3f &up);
	void Update();

private:
	bool HasChanged();
	mat4f GetParentModelMatrix();

public:
	inline vec3f GetPosition() { return Position; }
	inline quatf GetRotation() { return Rotation; }
	inline vec3f GetScale() { return Scale; }
	inline Transform* GetParent() { return Parent; }

	inline void SetPosition(vec3f &pos) { Position = pos; }
	inline void SetRotation(quatf &rot) { Rotation = rot; }
	inline void SetScale(vec3f &scale) { Scale = scale; }
	inline void SetParent(Transform *trans) { Parent = trans; }

private:
	vec3f Position;
	quatf Rotation;
	vec3f Scale;

	vec3f OldPosition;
	quatf OldRotation;
	vec3f OldScale;

	Transform *Parent;

	mat4f ParentMatrix;

	bool InitializeOldStuff;
};