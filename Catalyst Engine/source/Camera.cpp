#include <Camera.h>

void Camera::Update()
{
}

void Camera::SaveAndReset()
{
	OldView = ActiveView;
	ActiveView = ViewData{};
}

void Camera::Restore()
{
	ActiveView = OldView;
	OldView = ViewData{};
}

mat4f Camera::GetViewMatrix()
{
	mat4f trans = Mat4::Translation(ActiveView.Position * -1.0f);
	mat4f rot = Mat4::LookAt(ActiveView.Position, ActiveView.Target + ActiveView.Position, ActiveView.Up);

	return trans * rot;
}

mat4f Camera::GetProjectionMatrix()
{
	if (ActiveView.Mode == "ORTHO")
	{
		mat4f proj = Mat4::Orthographic(Projection.Width, Projection.Height, Projection.Near, Projection.Far);

		return Mat4::Scale(ActiveView.Scale) * proj;
	}
	else if (ActiveView.Mode == "PERSPECTIVE")
	{
		mat4f proj = Mat4::Perspective(Projection.FieldOfView, Projection.Width, Projection.Height, Projection.Near, Projection.Far);

		return Mat4::Scale(ActiveView.Scale) * proj;
	}

	return mat4f();
}

void Camera::SetViewData(vec3f &pos, vec3f &target, vec3f &up, vec3f &scale, char *mode)
{
	ActiveView = ViewData{ pos, target, up, scale, mode };
}

void Camera::SetProjectionData(const float &fov, const float &width, const float &height, const float &near, const float &far)
{
	Projection = ProjectionData{ fov, width, height, near, far };
}