#pragma once

#include <cinder\CinderMath.h>

using namespace Cinder::Math;

struct ViewData
{
	vec3f Position;
	vec3f Target;
	vec3f Up;
	vec3f Scale;

	char *Mode;
};

struct ProjectionData
{
	float FieldOfView;
	float Width;
	float Height;
	float Near;
	float Far;
};

class Camera
{
public:
	Camera()
		: ActiveView(ViewData{}),
		OldView(ViewData{}),
		Projection(ProjectionData{}),
		Locked(false)
	{}

	Camera(const vec3f &pos, const vec3f &target, const vec3f &up, const vec3f scale, char *mode)
		: ActiveView(ViewData{ pos, target, up, scale, mode }),
		OldView(ViewData{}),
		Projection(ProjectionData{}),
		Locked(false)
	{
	}

	~Camera() {}

public:
	void Update();
	void SaveAndReset();
	void Restore();
	mat4f GetViewMatrix();
	mat4f GetProjectionMatrix();

public:
	inline vec3f GetPosition() { return ActiveView.Position; }
	inline vec3f GetTarget() { return ActiveView.Target; }
	inline vec3f GetUp() { return ActiveView.Up; }
	inline vec3f GetScale() { return ActiveView.Scale; }
	inline char* GetMode() { return ActiveView.Mode; }

	inline ViewData GetActiveView() { return ActiveView; }
	inline ViewData GetOldView() { return OldView; }

	inline bool IsLocked() { return Locked; }

	inline void SetPosition(vec3f &pos) { ActiveView.Position = pos; }
	inline void SetTarget(vec3f &target) { ActiveView.Target = target; }
	inline void SetUp(vec3f &up) { ActiveView.Up = up; }
	inline void SetScale(vec3f &scale) { ActiveView.Scale = scale; }
	inline void SetMode(char *mode) { ActiveView.Mode = mode; }

	void SetViewData(vec3f &pos, vec3f &target, vec3f &up, vec3f &scale, char *mode);

	void SetProjectionData(const float &fov, const float &width, const float &height, const float &near, const float &far);
	inline void SetLock(bool lock) { Locked = lock; }

private:
	ProjectionData Projection;
	ViewData ActiveView;
	ViewData OldView;
	bool Locked;
};
