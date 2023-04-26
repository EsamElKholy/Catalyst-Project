#include <Transform.h>

void Transform::Update()
{
	if (InitializeOldStuff)
	{
		OldPosition = Position;
		OldRotation = Rotation;
		OldScale = Scale;

		// NOTE(KAI): TEMPORARY FIX FOR THE UPDATE LAG
		if (Parent)
		{
			ParentMatrix = Parent->GetModelMatrix();
		}
	}
	else
	{
		OldPosition = Position + vec3f(1, 1, 1);
		OldRotation = Rotation * 0.5f;
		OldPosition = Scale + vec3f(1, 1, 1);
		InitializeOldStuff = true;

		// NOTE(KAI): TEMPORARY FIX FOR THE UPDATE LAG
		if (Parent)
		{
			ParentMatrix = Parent->GetModelMatrix();
		}
	}
}

bool Transform::HasChanged()
{
	if (Parent != 0) 
	{
		if (Parent->HasChanged())
		{
			return true;
		}
	}

	if (Position != OldPosition) 
	{
		return true;
	}

	if (Rotation != OldRotation) 
	{
		return true;
	}

	if (Scale != OldScale) 
	{
		return true;
	}

	return false;
}

mat4f Transform::GetModelMatrix()
{
	mat4f trans = Mat4::Translation(Position);
	mat4f rot = Rotation.ToRotationMatrix();
	mat4f scale = Mat4::Scale(Scale);

	return scale * rot * trans * GetParentModelMatrix();
}

void Transform::Rotate(float angle, vec3f &axis)
{
	quatf q(angle, axis);

	Rotation = Quat::Normalize(q * Rotation);
}

void Transform::Translate(float amount, vec3f &axis)
{
	Position = Position + (amount * axis);
}

void Transform::LookAt(vec3f &point, vec3f &up)
{
	vec3f dir = Vec3::Normalize(point - Position);

	mat4f rotMat = Mat4::Rotation(dir, up);

	Rotation = quatf(rotMat);
}

mat4f Transform::GetParentModelMatrix()
{
	if (Parent != 0) 
	{
		if (Parent->HasChanged())
		{
			ParentMatrix = Parent->GetModelMatrix();
		}
	}

	return ParentMatrix;
}
