#include <IntersectionTest.h>

std::pair<float, bool> IntersectionTest::OBBTest(vec3f &origin, vec3f &direction, vec3f &minBounds, vec3f &maxBounds, Transform *trans, Transform *parent)
{
	float tMin = 0.0f;
	float tMax = 10000000.0f;

	bool intersected = false;

	mat4f t = Mat4::Translation(trans->GetPosition());
	mat4f r = trans->GetRotation().ToRotationMatrix();
	mat4f p;

	if (parent)
	{
		p = parent->GetModelMatrix();
	}

	mat4f transMat = (r * t) * p;

	vec3f obbCenter = vec3f(transMat.Elements_2D[3][0], transMat.Elements_2D[3][1], transMat.Elements_2D[3][2]);

	vec3f delta = obbCenter - origin;

	///////////////////////////////////////////////////////

	vec3f xAxis = vec3f(transMat.Elements_2D[0][0], transMat.Elements_2D[0][1], transMat.Elements_2D[0][2]);

	float e = Vec3::Dot(xAxis, delta);
	float f = Vec3::Dot(xAxis, direction);

	if (std::abs(f) > 0.000000001f)
	{
		float t1 = (e + (trans->GetScale().X * minBounds.X)) / f;
		float t2 = (e + (trans->GetScale().X * maxBounds.X)) / f;

		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t2 < tMax)
		{
			tMax = t2;
		}

		if (t1 > tMin)
		{
			tMin = t1;
		}

		if (tMin < tMax)
		{
			intersected = true;
		}
		else
		{
			intersected = false;
		}
	}
	else
	{
		if (-e + (trans->GetScale().X * minBounds.X) > 0.0f
			|| -e + (trans->GetScale().X * maxBounds.X))
		{
			intersected = false;
		}
	}

	////////////////////////////////////////////////////////////

	vec3f yAxis = vec3f(transMat.Elements_2D[1][0], transMat.Elements_2D[1][1], transMat.Elements_2D[1][2]);

	float e1 = Vec3::Dot(yAxis, delta);
	float f1 = Vec3::Dot(yAxis, direction);

	if (std::abs(f1) > 0.000000001f)
	{
		float t1 = (e1 + (trans->GetScale().Y * minBounds.Y)) / f1;
		float t2 = (e1 + (trans->GetScale().Y * maxBounds.Y)) / f1;

		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t2 < tMax)
		{
			tMax = t2;
		}

		if (t1 > tMin)
		{
			tMin = t1;
		}

		if (tMin < tMax)
		{
			intersected = true;
		}
		else
		{
			intersected = false;
		}
	}
	else
	{
		if (-e1 + (trans->GetScale().Y * minBounds.Y) > 0.0f
			|| -e1 + (trans->GetScale().Y * maxBounds.Y))
		{
			intersected = false;
		}
	}

	////////////////////////////////////////////////////////////////

	vec3f zAxis = vec3f(transMat.Elements_2D[2][0], transMat.Elements_2D[2][1], transMat.Elements_2D[2][2]);

	float e2 = Vec3::Dot(zAxis, delta);
	float f2 = Vec3::Dot(zAxis, direction);

	if (std::abs(f2) > 0.000000001f)
	{
		float t1 = (e2 + (trans->GetScale().Z * minBounds.Z)) / f2;
		float t2 = (e2 + (trans->GetScale().Z * maxBounds.Z)) / f2;

		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t2 < tMax)
		{
			tMax = t2;
		}

		if (t1 > tMin)
		{
			tMin = t1;
		}

		if (tMin < tMax)
		{
			intersected = true;
		}
		else
		{
			intersected = false;
		}
	}
	else
	{
		if (-e2 + (trans->GetScale().Z * minBounds.Z) > 0.0f
			|| -e2 + (trans->GetScale().Z * maxBounds.Z))
		{
			intersected = false;
		}
	}

	///////////////////////////////////////////////////

	std::pair<float, bool> res;

	res.first = tMin;
	res.second = intersected;

	return res;
}