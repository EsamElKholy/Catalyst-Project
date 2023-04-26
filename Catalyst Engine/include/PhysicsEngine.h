#pragma once

#include <string>
#include <vector>
#include "Transform.h"
#include "IComponent.h"
#include "Actor.h"

class Rigidbody
{
public:
	Rigidbody() {}
	~Rigidbody() {}

public:
	void Integrate(float delta)
	{
		if (InverseMass <= 0.0f)
		{
			return;
		}

		LastFrameAcceleration = Acceleration;
		LastFrameAcceleration += ForceAccumulator * InverseMass;

		vec3f torqueAccel = WorldInverseInertiaTensor() * TorqueAccumulator;

		Velocity = Velocity + (LastFrameAcceleration * delta);
		Rotation = Rotation + (torqueAccel * delta);

		Velocity *= powf(Damping, delta);
		Rotation *= powf(AngularDamping, delta);


		SetPosition(GetPosition() + (Velocity * delta));
	
		/////////
		{
			quatf q = quatf(Rotation * delta, 0);
			q *= RigidbodyTransform->GetRotation();

			float x = RigidbodyTransform->GetRotation().X + (q.X / 2.0f);
			float y = RigidbodyTransform->GetRotation().Y + (q.Y / 2.0f);
			float z = RigidbodyTransform->GetRotation().Z + (q.Z / 2.0f);
			float w = RigidbodyTransform->GetRotation().W + (q.W / 2.0f);

			SetAngularVelocity(Quat::Normalize(quatf(x, y, z, w)));
		}
		/////////
		
		//InverseInertiaTensor = WorldInverseInertiaTensor();

		ClearAccumulator();
	}

	void AddForce(vec3f &force) { ForceAccumulator += force; }

	void AddForceAtWorldPoint(vec3f &force, vec3f &point) 
	{
		vec3f pt = point;

		pt = pt - GetPosition();

		ForceAccumulator += force;
		TorqueAccumulator += Vec3::Cross(pt, force);
	}

	void AddForceAtBodyPoint(vec3f &force, vec3f &point)
	{
		vec3f pt = GetPointInWorldSpace(point);

		AddForceAtWorldPoint(force, pt);
	}

public:
	vec3f GetPosition() { return RigidbodyTransform->GetPosition(); }
	quatf GetAngularVelocity() { return RigidbodyTransform->GetRotation(); }
	vec3f GetRotation() { return Rotation; }

	vec3f GetPointInWorldSpace(vec3f &point) { return RigidbodyTransform->GetModelMatrix() * point; }
	vec3f GetPointInLocalSpace(vec3f &point) { return RigidbodyTransform->GetModelMatrix().Inverse() * point; }
	
	vec3f GetDirectionInWorldSpace(vec3f &dir) 
	{ 
		mat4f rot = RigidbodyTransform->GetRotation().ToRotationMatrix();
		
		return rot * dir; 
	}

	vec3f GetDirectionInLocalSpace(vec3f &dir) 
	{
		mat4f rot = RigidbodyTransform->GetRotation().ToRotationMatrix();

		return rot.Inverse() * dir;
	}

	Transform* GetTransform() { return RigidbodyTransform; }

	vec3f GetVelocity() { return Velocity; }
	vec3f GetAcceleration() { return Acceleration; }
	vec3f GetLastFrameAcceleration() { return LastFrameAcceleration; }
	float GetDamping() { return Damping; }
	float GetAngularDamping() { return AngularDamping; }
	float GetMass()
	{
		if (InverseMass <= 0.0f)
		{
			return InverseMass;
		}
		else
		{
			return 1.0f / InverseMass;
		}
	}

	float GetInverseMass() { return InverseMass; }

	bool HasInfiniteMass()
	{
		if (InverseMass <= 0.0f)
		{
			return true;
		}

		return false;
	}

public:
	void SetPosition(vec3f &pos) { RigidbodyTransform->SetPosition(pos); }
	void SetAngularVelocity(quatf &rot) { RigidbodyTransform->SetRotation(rot); }
	void SetRotation(vec3f &rot) { Rotation = rot; }

	void SetTransform(Transform *trans) { RigidbodyTransform = trans; }

	void SetVelocity(vec3f &vel) { Velocity = vel; }
	void SetAcceleration(vec3f &accel) { Acceleration = accel; }
	void SetDamping(float damping) { Damping = damping; }
	void SetAngularDamping(float angularDamping) { AngularDamping = angularDamping; }
	void SetMass(float mass)
	{
		if (mass <= 0.0f)
		{
			InverseMass = mass;
		}
		else
		{
			InverseMass = 1.0f / mass;
		}
	}

	void SetInverseMass(float invMass) { InverseMass = invMass; }

	void SetInertiaTensor(mat4f &it) 
	{
		InverseInertiaTensor = it.Inverse();

		//InverseInertiaTensor = WorldInverseInertiaTensor();
	}

	mat4f WorldInverseInertiaTensor() 
	{
		mat4f rot = RigidbodyTransform->GetRotation().ToRotationMatrix();

		return rot * InverseInertiaTensor;
	}

private:
	void ClearAccumulator()
	{
		ForceAccumulator = vec3f();
		TorqueAccumulator = vec3f();
	}

	void CalculateTransformMat() 
	{
		TransformMat = RigidbodyTransform->GetModelMatrix();
	}

private:
	Transform *RigidbodyTransform;
	mat4f TransformMat;
	mat4f InverseInertiaTensor;
	vec3f Rotation;
	vec3f Velocity;
	vec3f Acceleration;
	vec3f LastFrameAcceleration;
	vec3f ForceAccumulator;
	vec3f TorqueAccumulator;
	float Damping;
	float AngularDamping;
	float InverseMass;
};

class ForceGenerator
{
public:
	virtual void UpdateForce(Rigidbody *rb, float delta) = 0;
};

class ForceRegistry
{
public:
	ForceRegistry() {}
	~ForceRegistry() {}

public:
	void Add(Rigidbody *rb, ForceGenerator *fg)
	{
		if (rb && fg)
		{
			ForceRegistration r = { rb, fg };
			Registrations.push_back(r);
		}
	}

	void Remove(Rigidbody *rb, ForceGenerator *fg)
	{
		uint32 index = Registrations.size();

		for (uint32 i = 0; i < Registrations.size(); i++)
		{
			if (Registrations[i].RB == rb && Registrations[i].FG == fg)
			{
				index = i;

				break;
			}
		}

		if (index < Registrations.size())
		{
			Registrations.erase(Registrations.begin() + index);
		}
	}

	void ClearRegistration()
	{
		Registrations.clear();
	}

	void UpdateForces(float delta)
	{
		for (uint32 i = 0; i < Registrations.size(); i++)
		{
			Registrations[i].FG->UpdateForce(Registrations[i].RB, delta);
		}
	}

private:
	struct ForceRegistration
	{
		Rigidbody *RB;
		ForceGenerator *FG;
	};

	typedef std::vector<ForceRegistration> Registry;
	Registry Registrations;
};

class Gravity : public ForceGenerator
{
public:
	Gravity(vec3f &gravity)
		: GravityAccel(gravity)
	{}
	~Gravity() {}

public:
	virtual void UpdateForce(Rigidbody *rb, float delta) override
	{
		if (rb->HasInfiniteMass())
		{
			return;
		}

		rb->AddForce(GravityAccel * rb->GetMass());
	}

private:
	vec3f GravityAccel;
};

class Drag : public ForceGenerator
{
public:
	Drag(float k1, float k2)
		: K1(k1), K2(k2)
	{}
	~Drag() {}

public:
	virtual void UpdateForce(Rigidbody *rb, float delta) override
	{
		vec3f force = rb->GetVelocity();

		float speed = Vec3::Length(force);

		float dragCoeff = (K1 * speed) + (K2 * speed * speed);

		if (speed > 0.0f)
		{
			force = Vec3::Normalize(force);
		}

		force *= -dragCoeff;

		rb->AddForce(force);
	}

private:
	float K1;
	float K2;
};

class Spring : public ForceGenerator
{
public:
	Spring(Rigidbody *other, vec3f &cp, vec3f ocp, float k, float l)
		: Other(other)
		, ConnectionPoint(cp), OtherConnectionPoint(ocp)
		, SpringConstant(k), RestLength(l)
	{}
	~Spring() {}

public:
	virtual void UpdateForce(Rigidbody *rb, float delta) override
	{
		vec3f firstPos = rb->GetPointInWorldSpace(ConnectionPoint);
		vec3f secondPos = Other->GetPointInWorldSpace(OtherConnectionPoint);

		vec3f force = firstPos - secondPos;

		float magnitude = Vec3::Length(force);

		magnitude = fabsf(magnitude - RestLength);

		magnitude *= SpringConstant;

		force = Vec3::Normalize(force);

		force *= -magnitude;

		rb->AddForceAtWorldPoint(force, firstPos);
	}

private:
	Rigidbody *Other;
	vec3f ConnectionPoint;
	vec3f OtherConnectionPoint;
	float SpringConstant;
	float RestLength;
};

class AnchoredSpring : public ForceGenerator
{
public:
	AnchoredSpring(vec3f &anchor, vec3f &cp, float k, float l)
		: Anchor(anchor), ConnectionPoint(cp), SpringConstant(k), RestLength(l)
	{}
	~AnchoredSpring() {}

public:
	virtual void UpdateForce(Rigidbody *rb, float delta) override
	{
		vec3f cp = rb->GetPointInWorldSpace(ConnectionPoint);
		vec3f force = cp - Anchor;

		float magnitude = Vec3::Length(force);

		magnitude = fabsf(magnitude - RestLength);

		magnitude *= SpringConstant;

		force = Vec3::Normalize(force);

		force *= -magnitude;

		rb->AddForceAtWorldPoint(force, cp);
	}

private:
	vec3f Anchor;
	vec3f ConnectionPoint;
	float SpringConstant;
	float RestLength;
};

class Bungee : public ForceGenerator
{
public:
	Bungee(Rigidbody *other, vec3f &cp, vec3f ocp, float k, float l)
		: Other(other)
		, ConnectionPoint(cp), OtherConnectionPoint(ocp)
		, SpringConstant(k), RestLength(l)
	{}
	~Bungee() {}

public:
	virtual void UpdateForce(Rigidbody *rb, float delta) override
	{
		vec3f firstPos = rb->GetPointInWorldSpace(ConnectionPoint);
		vec3f secondPos = Other->GetPointInWorldSpace(OtherConnectionPoint);

		vec3f force = firstPos - secondPos;

		float magnitude = Vec3::Length(force);

		if (magnitude <= RestLength)
		{
			return;
		}

		magnitude = (magnitude - RestLength);

		magnitude *= SpringConstant;

		force = Vec3::Normalize(force);

		force *= -magnitude;

		rb->AddForceAtWorldPoint(force, firstPos);
	}

private:
	Rigidbody *Other;
	vec3f ConnectionPoint;
	vec3f OtherConnectionPoint;
	float SpringConstant;
	float RestLength;
};

class AnchoredBungee : public ForceGenerator
{
public:
	AnchoredBungee(vec3f &anchor, vec3f &cp, float k, float l)
		: Anchor(anchor), ConnectionPoint(cp), SpringConstant(k), RestLength(l)
	{}
	~AnchoredBungee() {}

public:
	virtual void UpdateForce(Rigidbody *rb, float delta) override
	{
		vec3f cp = rb->GetPointInWorldSpace(ConnectionPoint);
		vec3f force = cp - Anchor;

		float magnitude = Vec3::Length(force);

		if (magnitude <= RestLength)
		{
			return;
		}

		magnitude = (magnitude - RestLength);

		magnitude *= SpringConstant;

		force = Vec3::Normalize(force);

		force *= -magnitude;

		rb->AddForceAtWorldPoint(force, cp);
	}

private:
	vec3f Anchor;
	vec3f ConnectionPoint;
	float SpringConstant;
	float RestLength;
};

class Buoyancy : public ForceGenerator
{
public:
	Buoyancy(float maxDepth, float volume, float liquidHeight, float density = 1000.0f)
		: MaxDepth(maxDepth), Volume(volume), LiquidHeight(liquidHeight), LiquidDensity(density)
	{}
	~Buoyancy() {}

public:
	virtual void UpdateForce(Rigidbody *rb, float delta) override
	{
		float depth = rb->GetPosition().Y;

		if (depth >= LiquidHeight + MaxDepth)
		{
			return;
		}

		vec3f force;

		if (depth <= LiquidHeight - MaxDepth)
		{
			force.Y = LiquidDensity * Volume;

			rb->AddForce(force);

			return;
		}

		force.Y = (LiquidDensity * Volume) * (depth - MaxDepth - LiquidHeight) / (2.0f * MaxDepth);
		rb->AddForce(force);
	}

private:
	float MaxDepth;
	float Volume;
	float LiquidHeight;
	float LiquidDensity;
};

///////////////////////////////////////////////////////////////////////////////
class ContactResolver;

class Contact
{
	friend class ContactResolver;

public:
	Contact() {}
	~Contact() {}

public:
	void SetFriction(float f) { Friction = f; }
	void SetPenetration(float p) { Penetration = p; }
	void SetRestitution(float r) { Restitution = r; }
	void SetContactNormal(vec3f cn) { ContactNormal = cn; }
	void SetContactPoint(vec3f cp) { ContactPoint = cp; }
	void SetRigidbodies(Rigidbody *rb1, Rigidbody *rb2) { Rigidbodies[0] = rb1; Rigidbodies[1] = rb2; }

	float GetFriction() { return Friction; }
	float GetPenetration() { return Penetration; }
	float GetRestitution() { return Restitution; }
	Rigidbody* GetFirstRigidbody() { return Rigidbodies[0]; }
	Rigidbody* GetSecondRigidbody() { return Rigidbodies[1]; }
	Rigidbody** GetRigidbodies() { return Rigidbodies; }
	vec3f GetContactNormal() { return ContactNormal; }
	vec3f GetContactPoint() { return ContactPoint; }

private:
	void CalculateInternals(float delta) 
	{
		if (!Rigidbodies[0])
		{
			SwapBodies();
		}

		CalculateContactBasis();

		RelativeContactPositions[0] = ContactPoint - Rigidbodies[0]->GetPosition();

		if (Rigidbodies[1])
		{
			RelativeContactPositions[1] = ContactPoint - Rigidbodies[1]->GetPosition();
		}

		ContactVelocity = CalculateLocalVelocity(0, delta);

		if (Rigidbodies[1])
		{
			ContactVelocity -= CalculateLocalVelocity(1, delta);
		}

		CalculateDesiredDeltaVelocity(delta);
	}

	void SwapBodies() 
	{
		ContactNormal *= -1.0f;
		Rigidbody *temp = Rigidbodies[0];
		Rigidbodies[0] = Rigidbodies[1];
		Rigidbodies[1] = temp;
	}

	void CalculateDesiredDeltaVelocity(float delta) 
	{
		float velocityLimit = 0.1;

		float velocityFromAccel = Vec3::Dot(Rigidbodies[0]->GetLastFrameAcceleration() * delta, ContactNormal);

		if (Rigidbodies[1])
		{
			velocityFromAccel -= Vec3::Dot(Rigidbodies[1]->GetLastFrameAcceleration() * delta, ContactNormal);
		}

		float limitRestitution = Restitution;

		if (fabsf(ContactVelocity.X) < velocityLimit)
		{
			limitRestitution = 0.0f;
		}

		DesiredDeltaVelocity = -ContactVelocity.X - limitRestitution * (ContactVelocity.X - velocityFromAccel);
	}
	
	vec3f CalculateLocalVelocity(uint32 bodyIndex, float delta) 
	{
		Rigidbody *body = Rigidbodies[bodyIndex];

		vec3f velocity = Vec3::Cross(body->GetRotation(), RelativeContactPositions[bodyIndex]);
		velocity += body->GetVelocity();

		mat3f tc = ContactToWorld.Transpose();

		vec3f contactVelocity = tc * velocity;

		vec3f accelVel = body->GetLastFrameAcceleration() * delta;

		accelVel = tc * accelVel;

		accelVel.X = 0;

		contactVelocity += accelVel;

		return contactVelocity;
	}

	void CalculateContactBasis() 
	{
		vec3f contactTangent[2];

		if (fabsf(ContactNormal.X) > fabsf(ContactNormal.Y))
		{
			float s = 1.0f / sqrtf((ContactNormal.Z * ContactNormal.Z) + (ContactNormal.X * ContactNormal.X));

			contactTangent[0].X = ContactNormal.X * s;
			contactTangent[0].Y = 0;
			contactTangent[0].Z = -ContactNormal.Z * s;

			contactTangent[1].X = ContactNormal.Y * contactTangent[0].X;
			contactTangent[1].Y = ContactNormal.Z * contactTangent[0].X -
				ContactNormal.X * contactTangent[0].Z;
			contactTangent[1].Z = -ContactNormal.Y * contactTangent[0].X;
		}
		else
		{
			float s = 1.0f / sqrtf((ContactNormal.Z * ContactNormal.Z) + (ContactNormal.Y * ContactNormal.Y));

			contactTangent[0].X = 0;
			contactTangent[0].Y = -ContactNormal.Z * s;
			contactTangent[0].Z = ContactNormal.Y * s;

			contactTangent[1].X = ContactNormal.Y * contactTangent[0].Z -
				ContactNormal.Z * contactTangent[0].Y;
			contactTangent[1].Y = -ContactNormal.X * contactTangent[0].Z;
			contactTangent[1].Z = ContactNormal.X * contactTangent[0].Y;
		}

		ContactToWorld.Elements_2D[0][0] = ContactNormal.X;
		ContactToWorld.Elements_2D[0][1] = ContactNormal.Y;
		ContactToWorld.Elements_2D[0][2] = ContactNormal.Z;

		ContactToWorld.Elements_2D[1][0] = contactTangent[0].X;
		ContactToWorld.Elements_2D[1][1] = contactTangent[0].Y;
		ContactToWorld.Elements_2D[1][2] = contactTangent[0].Z;

		ContactToWorld.Elements_2D[2][0] = contactTangent[1].X;
		ContactToWorld.Elements_2D[2][1] = contactTangent[1].Y;
		ContactToWorld.Elements_2D[2][2] = contactTangent[1].Z;
	}

	void ApplyImpulse(vec3f &impulse, Rigidbody *body, vec3f *velChange, vec3f *rotChange) 
	{
	
	}
	
	void ApplyVelocityChange(vec3f velChange[2], vec3f rotChange[2]) 
	{
		mat4f inverseInertiaTensor[2];

		inverseInertiaTensor[0] = Rigidbodies[0]->WorldInverseInertiaTensor();

		if (Rigidbodies[1])
		{
			inverseInertiaTensor[1] = Rigidbodies[1]->WorldInverseInertiaTensor();
		}

		vec3f impuleContact = CalculateFrictionlessImpulse(inverseInertiaTensor);

		vec3f impulse = ContactToWorld * impuleContact;

		vec3f impulsiveTorque = Vec3::Cross(RelativeContactPositions[0], impulse);
		rotChange[0] = inverseInertiaTensor[0] * impulsiveTorque;
		velChange[0] = impulse * Rigidbodies[0]->GetInverseMass();

		Rigidbodies[0]->SetVelocity(Rigidbodies[0]->GetVelocity() + velChange[0]);
		Rigidbodies[0]->SetRotation(Rigidbodies[0]->GetRotation() + rotChange[0]);

		if (Rigidbodies[1])
		{
			impulsiveTorque = Vec3::Cross(impulse, RelativeContactPositions[1]);
			rotChange[1] = inverseInertiaTensor[1] * impulsiveTorque;
			velChange[1] = impulse * -Rigidbodies[1]->GetInverseMass();

			Rigidbodies[1]->SetVelocity(Rigidbodies[1]->GetVelocity() + velChange[1]);
			Rigidbodies[1]->SetRotation(Rigidbodies[1]->GetRotation() + rotChange[1]);
		}
	}

	void ApplyPositionChange(vec3f linearChange[2], vec3f angularChange[2], float penetration) 
	{
		float angularLimit = 0.000025;

		float angularMove[2];
		float linearMove[2];

		float totalInertia = 0;
		float linearInertia[2];
		float angularInertia[2];

		for (uint32 i = 0; i < 2; i++)
		{
			if (Rigidbodies[i])
			{
				mat4f inverseInertiaTensor = Rigidbodies[i]->WorldInverseInertiaTensor();

				vec3f angularInertiaWorld = Vec3::Cross(RelativeContactPositions[i], ContactNormal);
				angularInertiaWorld = inverseInertiaTensor * angularInertiaWorld;
				angularInertiaWorld = Vec3::Cross(angularInertiaWorld, RelativeContactPositions[i]);

				angularInertia[i] = Vec3::Dot(angularInertiaWorld, ContactNormal);

				linearInertia[i] = Rigidbodies[i]->GetInverseMass();

				totalInertia += linearInertia[i] + angularInertia[i];
			}
		}

		// Loop through again calculating and applying the changes
		for (unsigned i = 0; i < 2; i++)
		{
			if (Rigidbodies[i])
			{
				// The linear and angular movements required are in proportion to
				// the two inverse inertias.
				float sign = (i == 0) ? 1 : -1;

				angularMove[i] =
					sign * penetration * (angularInertia[i] / totalInertia);
				linearMove[i] =
					sign * penetration * (linearInertia[i] / totalInertia);

				// To avoid angular projections that are too great (when mass is large
				// but inertia tensor is small) limit the angular move.
				vec3f projection = RelativeContactPositions[i];
				projection +=
					ContactNormal *
					-Vec3::Dot(RelativeContactPositions[i], (ContactNormal));

				// Use the small angle approximation for the sine of the angle (i.e.
				// the magnitude would be sine(angularLimit) * projection.magnitude
				// but we approximate sine(angularLimit) to angularLimit).
				float maxMagnitude = angularLimit * Vec3::Length(projection);

				if (angularMove[i] < -maxMagnitude)
				{
					float totalMove = angularMove[i] + linearMove[i];
					angularMove[i] = -maxMagnitude;
					linearMove[i] = totalMove - angularMove[i];
				}
				else if (angularMove[i] > maxMagnitude)
				{
					float totalMove = angularMove[i] + linearMove[i];
					angularMove[i] = maxMagnitude;
					linearMove[i] = totalMove - angularMove[i];
				}

				// We have the linear amount of movement required by turning
				// the rigid body (in angularMove[i]). We now need to
				// calculate the desired rotation to achieve that.
				if (angularMove[i] == 0)
				{
					// Easy case - no angular movement means no rotation.
					angularChange[i] = vec3f();
				}
				else
				{
					// Work out the direction we'd like to rotate in.
					vec3f targetAngularDirection =
						Vec3::Cross(RelativeContactPositions[i], ContactNormal);

					mat4f inverseInertiaTensor;
					Rigidbodies[i]->WorldInverseInertiaTensor();

					// Work out the direction we'd need to rotate to achieve that
					angularChange[i] =
						(inverseInertiaTensor * targetAngularDirection) *
						(angularMove[i] / angularInertia[i]);
				}

				// Velocity change is easier - it is just the linear movement
				// along the contact normal.
				linearChange[i] = ContactNormal * linearMove[i];

				// Now we can start to apply the values we've calculated.
				// Apply the linear movement
				vec3f pos;
				pos = Rigidbodies[i]->GetPosition();
				pos += ContactNormal * linearMove[i];
				Rigidbodies[i]->SetPosition(pos);

				// And the change in orientation
				quatf q = quatf(angularChange[i], 0);
				q *= Rigidbodies[i]->GetAngularVelocity();

				float x = Rigidbodies[i]->GetAngularVelocity().X + (q.X / 2.0f);
				float y = Rigidbodies[i]->GetAngularVelocity().Y + (q.Y / 2.0f);
				float z = Rigidbodies[i]->GetAngularVelocity().Z + (q.Z / 2.0f);
				float w = Rigidbodies[i]->GetAngularVelocity().W + (q.W / 2.0f);

				//Rigidbodies[i]->SetAngularVelocity(Quat::Normalize(quatf(x, y, z, w)));
			}
		}
	}
	
	vec3f CalculateFrictionlessImpulse(mat4f *inverseInertiaTensor) 
	{
		vec3f impulseContact;

		vec3f deltaVelWorld = Vec3::Cross(RelativeContactPositions[0], ContactNormal);
		deltaVelWorld = inverseInertiaTensor[0] * deltaVelWorld;
		deltaVelWorld = Vec3::Cross(deltaVelWorld, RelativeContactPositions[0]);

		float deltaVel = Vec3::Dot(deltaVelWorld, ContactNormal);
		deltaVel += Rigidbodies[0]->GetInverseMass();

		if (Rigidbodies[1])
		{
			deltaVelWorld = Vec3::Cross(RelativeContactPositions[1], ContactNormal);
			deltaVelWorld = inverseInertiaTensor[1] * deltaVelWorld;
			deltaVelWorld = Vec3::Cross(deltaVelWorld, RelativeContactPositions[1]);

			deltaVel += Vec3::Dot(deltaVelWorld, ContactNormal);
			deltaVel += Rigidbodies[1]->GetInverseMass();
		}

		impulseContact.X = DesiredDeltaVelocity / deltaVel;
		impulseContact.Y = 0;
		impulseContact.Z = 0;

		return impulseContact;
	}

private:
	Rigidbody *Rigidbodies[2];
	mat3f ContactToWorld;
	vec3f RelativeContactPositions[2];
	vec3f ContactPoint;
	vec3f ContactNormal;
	vec3f ContactVelocity;
	float DesiredDeltaVelocity;
	float Penetration;
	float Restitution;
	float Friction;
};

class ContactResolver
{
public:
	ContactResolver(uint32 it, float posE = 0.01f, float velE = 0.01f)
	{
		SetIterations(it, it);
		SetEpsilons(posE, velE);
	}

	~ContactResolver() {}

public:
	void ResolveContacts(Contact *contacts, uint32 contactCount, float delta)
	{
		// Make sure we have something to do.
		if (contactCount == 0) return;

		// Prepare the contacts for processing
		PrepareContacts(contacts, contactCount, delta);

		// Resolve the interpenetration problems with the contacts.
		AdjustPositions(contacts, contactCount, delta);

		// Resolve the velocity problems with the contacts.
		AdjustVelocities(contacts, contactCount, delta);
	}

public:
	void SetIterations(uint32 it)
	{
		PositionIterations = it;
		VelocityIterations = it;
	}

	void SetIterations(uint32 it1, uint32 it2)
	{
		PositionIterations = it1;
		VelocityIterations = it2;
	}

	void SetEpsilons(float e)
	{
		VelocityEpsilon = e;
		PositionEpsilon = e;
	}

	void SetEpsilons(float e1, float e2)
	{
		PositionEpsilon = e1;
		VelocityEpsilon = e2;
	}

private:
	void PrepareContacts(Contact *contacts, uint32 contactCount, float delta) 
	{
		// Generate contact velocity and axis information.
		Contact* lastContact = contacts + contactCount;
		for (Contact* contact = contacts; contact < lastContact; contact++)
		{
			// Calculate the internal contact data (inertia, basis, etc).
			contact->CalculateInternals(delta);
		}
	}

	void AdjustPositions(Contact *contacts, uint32 contactCount, float delta) 
	{
		uint32 i, index;
		vec3f linearChange[2], angularChange[2];
		float max;
		vec3f deltaPosition;

		// iteratively resolve interpenetrations in order of severity.
		PositionIterationsUsed = 0;
		while (PositionIterationsUsed < PositionIterations)
		{
			// Find biggest penetration
			max = PositionEpsilon;
			index = contactCount;
			for (i = 0; i < contactCount; i++)
			{
				if (contacts[i].GetPenetration() > max)
				{
					max = contacts[i].GetPenetration();
					index = i;
				}
			}
			if (index == contactCount) break;

			// Resolve the penetration.
			contacts[index].ApplyPositionChange(
				linearChange,
				angularChange,
				max);

			// Again this action may have changed the penetration of other
			// bodies, so we update contacts.
			for (i = 0; i < contactCount; i++)
			{
				// Check each body in the contact
				for (unsigned b = 0; b < 2; b++) if (contacts[i].Rigidbodies[b])
				{
					// Check for a match with each body in the newly
					// resolved contact
					for (unsigned d = 0; d < 2; d++)
					{
						if (contacts[i].Rigidbodies[b] == contacts[index].Rigidbodies[d])
						{
							deltaPosition = linearChange[d] +
								Vec3::Cross(angularChange[d], contacts[i].RelativeContactPositions[b]);

							// The sign of the change is positive if we're
							// dealing with the second body in a contact
							// and negative otherwise (because we're
							// subtracting the resolution)..
							contacts[i].SetPenetration(contacts[i].GetPenetration() +
								Vec3::Dot(deltaPosition, contacts[i].GetContactNormal())
								* (b ? 1.0f : -1.0f));
						}
					}
				}
			}
			PositionIterationsUsed++;
		}
	}

	void AdjustVelocities(Contact *contacts, uint32 contactCount, float delta) 
	{
		vec3f velocityChange[2], rotationChange[2];
		vec3f deltaVel;

		// iteratively handle impacts in order of severity.
		VelocityIterationsUsed = 0;
		while (VelocityIterationsUsed < VelocityIterations)
		{
			// Find contact with maximum magnitude of probable velocity change.
			float max = VelocityEpsilon;
			unsigned index = contactCount;
			for (unsigned i = 0; i < contactCount; i++)
			{
				if (contacts[i].DesiredDeltaVelocity > max)
				{
					max = contacts[i].DesiredDeltaVelocity;
					index = i;
				}
			}
			if (index == contactCount) break;

			// Do the resolution on the contact that came out top.
			contacts[index].ApplyVelocityChange(velocityChange, rotationChange);

			// With the change in velocity of the two bodies, the update of
			// contact velocities means that some of the relative closing
			// velocities need recomputing.
			for (unsigned i = 0; i < contactCount; i++)
			{
				// Check each body in the contact
				for (unsigned b = 0; b < 2; b++)
				{
					if (contacts[i].Rigidbodies[b])
					{
						// Check for a match with each body in the newly
						// resolved contact
						for (unsigned d = 0; d < 2; d++)
						{
							if (contacts[i].Rigidbodies[b] == contacts[index].Rigidbodies[d])
							{
								deltaVel = velocityChange[d] +
									Vec3::Cross(rotationChange[d], contacts[i].RelativeContactPositions[b]);

								// The sign of the change is negative if we're dealing
								// with the second body in a contact.
								contacts[i].ContactVelocity +=
									(contacts[i].ContactToWorld.Transpose() * deltaVel)
									* (b ? -1.0f : 1.0f);
								contacts[i].CalculateDesiredDeltaVelocity(delta);
							}
						}
					}
				}
			}
			VelocityIterationsUsed++;
		}
	}

private:
	uint32 PositionIterations;
	uint32 VelocityIterations;
	uint32 PositionIterationsUsed;
	uint32 VelocityIterationsUsed;
	float VelocityEpsilon;
	float PositionEpsilon;
};

class ContactGenerator
{
public:
	virtual void AddContact(Contact *c, uint32 limit) = 0;
};

class Link : public ContactGenerator
{
public:
	Link() {}
	~Link() {}

public:
	virtual void AddContact(Contact *c, uint32 limit) = 0;

public:
	float GetCurrentLength() 
	{
		vec3f relPos = Rigidbodies[0]->GetPosition() - Rigidbodies[1]->GetPosition();

		return Vec3::Length(relPos);
	}

protected:
	Rigidbody *Rigidbodies[2];
};

class Cable : public Link	
{
public:
	Cable() {}
	~Cable() {}

public:
	virtual void AddContact(Contact *c, uint32 limit) override
	{
		float length = GetCurrentLength();

		if (length < MaxLength)
		{
			return;
		}

		c->SetRigidbodies(Rigidbodies[0], Rigidbodies[1]);

		vec3f normal = Rigidbodies[1]->GetPosition() - Rigidbodies[0]->GetPosition();
		normal = Vec3::Normalize(normal);
		c->SetContactNormal(normal);
		c->SetPenetration(length - MaxLength);
		c->SetRestitution(Restitution);
	}

private:
	float Restitution;
	float MaxLength;
};

class Rod : public Link
{
public:
	Rod() {}
	~Rod() {}

public:
	virtual void AddContact(Contact *c, uint32 limit) override
	{
		float length = GetCurrentLength();

		if (length == MaxLength)
		{
			return;
		}

		c->SetRigidbodies(Rigidbodies[0], Rigidbodies[1]);

		vec3f normal = Rigidbodies[1]->GetPosition() - Rigidbodies[0]->GetPosition();
		normal = Vec3::Normalize(normal);
		if (length > MaxLength)
		{
			c->SetContactNormal(normal);
			c->SetPenetration(length - MaxLength);
		}
		else
		{
			normal *= -1.0f;
			c->SetContactNormal(normal);
			c->SetPenetration(MaxLength - length);
		}

		c->SetRestitution(0);
	}

private:
	float MaxLength;
};

/*class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();

public:
	void Init();
	void AddParticle(Particle *particle);
	void ConnectParticle();
	void AddForceGenerator(Particle *p, ParticleForceGenerator *g);

	void Clear();

	void Update(float delta);

private:
	uint32 GenerateContacts();
	void Integrate();

private:
	std::vector<ParticleContactGenerator*> ContactGenerators;
	std::vector<ParticleContact*> Contacts;
	std::vector<Particle*> Particles;
	ParticleContactResolver *ContactResolver;
	ParticleForceRegistry *ForceRegistry;
	uint32 MaxContacts;
};*/

struct PotentialContacts 
{
	Rigidbody *Body[2];
};

template<class BoundingVolumeClass>
class BVHNode
{
public:
	BVHNode(BVHNode *parent, BoundingVolumeClass &volume, Rigidbody *body) 
		: Parent(parent), BoundingVolume(volume), Body(body)
	{
		Children[0] = NULL;
		Children[1] = NULL;
	}

	~BVHNode() 
	{
		if (Parent)
		{
			BVHNode<BoundingVolumeClass> *sibling;

			if (Parent->Children[0] == this)
			{
				sibling = Parent->Children[1];
			}
			else
			{
				sibling = Parent->Children[0];
			}

			sibling->Parent = NULL;
			sibling->Body = NULL;
			sibling->Children[0] = NULL;
			sibling->Children[1] = NULL;
			
			delete sibling;

			RecalculateBoundingVolume();
		}
		
		if (Children[0])
		{
			Children[0]->Parent = NULL;
			delete Children[0];
		}

		if (Children[1])
		{
			Children[1]->Parent = NULL;
			delete Children[1];
		}
	}

public:
	bool IsLeaf() 
	{
		if (Body)
		{
			return false;
		}

		return true;	
	}

	uint32 GetPotentialContacts(PotentialContacts *contacts, uint32 limit) 
	{
		if (IsLeaf() || limit == 0)
		{
			return 0;
		}

		return Children[0]->GetPotentialContactWith(Children[1], contacts, limit);
	}

	uint32 GetPotentialContactWith(BVHNode<BoundingVolumeClass> *other, PotentialContacts *contacts, uint32 limit) 
	{
		if (!Overlaps(other) || limit == 0)
		{
			return 0;
		}

		if (IsLeaf() && other->IsLeaf())
		{
			contacts->Body[0] = Body;
			contacts->Body[1] = other->Body;

			return 1;
		}

		if (other->IsLeaf() || (!IsLeaf() && BoundingVolume->GetSize() >= other->BoundingVolume->GetSize()))
		{
			uint32 count = Children[0]->GetPotentialContactsWith(other, contacts, limit);

			if (limit > count)
			{
				return count + Children[1]->GetPotentialContactsWith(other, contacts + count, limit - count);
			}
			else
			{
				return count;
			}
		}
		else
		{
			uint32 count = GetPotentialContactsWith(other->Children[0], contacts, limit);

			if (limit > count)
			{
				return count + GetPotentialContactsWith(other->Children[1], contacts + count, limit - count);
			}
			else
			{
				return count;
			}
		}
	}

	void Insert(Rigidbody *body, BoundingVolumeClass &vol) 
	{
		if (IsLeaf())
		{
			Children[0] = new BVHNode<BoundingVolumeClass>(this, BoundingVolume, Body);
			Children[1] = new BVHNode<BoundingVolumeClass>(this, vol, body);

			this->Body = NULL;

			RecalculateBoundingVolume();
		}
		else
		{
			if (Children[0]->BoundingVolume.GetGrowth() < Children[1]->BoundingVolume.GetGrowth())
			{
				Children[0]->Insert(body, vol);
			}
			else
			{
				Children[1]->Insert(body, vol);
			}
		}
	}

	bool Overlaps(BVHNode<BoundingVolumeClass> *other) 
	{
		return BoundingVolume->Overlaps(other->BoundingVolume);
	}

public:
	void SetFirstChild(BVHNode *node) { Children[0] = node; }
	void SetSecondChild(BVHNode *node) { Children[1] = node; }

	BVHNode* GetFirstChild() { return Children[0]; }
	BVHNode* GetSecondChild() { return Children[1]; }

	void SetRigibody(Rigidbody *body) { Body = body; }
	Rigidbody* GetRigidbody() { return Body; }

private:
	void RecalculateBoundingVolume(bool recurse = true)
	{
		if (IsLeaf())
		{
			return;
		}

		BoundingVolume = BoundingVolumeClass(Children[0]->BoundingVolume, Children[1]->BoundingVolume);

		if (Parent)
		{
			Parent->RecalculateBoundingVolume(true);
		}
	}

private:
	BVHNode *Children[2];
	BVHNode *Parent;
	BoundingVolumeClass BoundingVolume;
	Rigidbody *Body;
};

class BoundingSphere
{
public:
	BoundingSphere(float rad, vec3f center) 
	{
		Radius = rad;			
		Center = center;
	}

	BoundingSphere(BoundingSphere &first, BoundingSphere &second) 
	{
		vec3f centerOffset = second.Center - first.Center;
		float distance = Vec3::LengthSquare(centerOffset);
		float radDiff = second.Radius - first.Radius;

		if ((radDiff * radDiff) >= distance)
		{
			if (first.Radius > second.Radius)
			{
				Center = first.Center;
				Radius = first.Radius;
			}
			else
			{
				Center = second.Center;
				Radius = second.Radius;
			}
		}
		else
		{
			distance = Vec3::Length(centerOffset);

			Radius = (first.Radius + second.Radius + distance) / 2.0f;

			Center = first.Center;

			if (distance  > 0)
			{
				Center += centerOffset + ((Radius - first.Radius) / distance);
			}
		}
	}

	~BoundingSphere() {}

public:
	bool Overlaps(BoundingSphere *other) 
	{
		float radSumSq = (Radius * other->Radius) * (Radius * other->Radius);
		float distSq = Vec3::LengthSquare(Center - other->Center);

		return distSq < radSumSq;
	}

public:
	void SetRaduis(float r) { Radius = r; }
	float GetRadius() { return Radius; }
	float GetSize() { return (4.0f / 3.0f) * PI * Radius * Radius * Radius; }

	float GetGrowth(BoundingSphere &other) 
	{
		BoundingSphere s(*this, other);

		return (s.Radius * s.Radius) - (Radius * Radius);
	}

private:
	vec3f Center;
	float Radius;
};

struct CollisionData
{
	std::vector<Contact> Contacts;

	float Restitution;
	float Friction;
	

	void AddContacts(Contact c) 
	{
		Contacts.push_back(c);
	}

	void Reset() 
	{
		Contacts.clear();
	}
};

class CollisionPrimitive
{
public:
	CollisionPrimitive() {}
	~CollisionPrimitive() {}

public:
	mat4f GetTransform() 
	{
		return Offset * Body->GetTransform()->GetModelMatrix();
	}

	Rigidbody* GetBody() { return Body; }
	void SetBody(Rigidbody *body) { Body = body; }

private:
	Rigidbody *Body;
	mat4f Offset;
};

class CollisionSphere : public CollisionPrimitive
{
public:
	CollisionSphere(float rad) 
		: Radius(rad)
	{}

	~CollisionSphere() {}

public:
	void SetRadius(float rad) { Radius = rad; }
	float GetRadius() { return Radius; }

private:
	float Radius;
};

class CollisionPlane : public CollisionPrimitive
{
public:
	CollisionPlane() {}
	CollisionPlane(vec3f normal, float offset)
		: Offset(offset), Normal(normal)
	{}

	~CollisionPlane() {}

public:
	void SetOffset(float offset) { Offset = offset; }
	float GetOffset() { return Offset; }

	void SetNormal(vec3f normal) { Normal = normal; }
	vec3f GetNormal() { return Normal; }

private:
	vec3f Normal;
	float Offset;
};

class CollisionBox : public CollisionPrimitive
{
public:
	CollisionBox(vec3f halfSize)
		: HalfSize(halfSize)
	{}

	~CollisionBox() {}

public:
	void SetHalfSize(vec3f halfSize) { HalfSize = halfSize; }
	vec3f GetHalfSize() { return HalfSize; }

private:
	vec3f HalfSize;
};

class CollisionDetector
{
public:
	CollisionDetector() {}
	~CollisionDetector() {}

public:
	uint32 SphereAndSphere(CollisionSphere &first, CollisionSphere &second, CollisionData *data) 
	{
		vec3f pos1 = first.GetTransform()[3].XYZ();
		vec3f pos2 = second.GetTransform()[3].XYZ();

		vec3f midline = pos1 - pos2;

		float size = Vec3::Length(midline);

		if (size <= 0.0f || size > first.GetRadius() + second.GetRadius())
		{
			return 0;
		}

		vec3f normal = midline * (1.0f / size);

		Contact contact;
		contact.SetContactNormal(normal);
		contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
		contact.SetContactPoint(pos1 + midline / 2.0f);
		contact.SetPenetration((first.GetRadius() + second.GetRadius()) - size);
		contact.SetRigidbodies(first.GetBody(), second.GetBody());
		contact.SetRestitution(data->Restitution);
		contact.SetFriction(data->Friction);

		data->AddContacts(contact);

		return 1;
	}

	uint32 SphereAndHalfSpace(CollisionSphere &first, CollisionPlane &second, CollisionData *data)
	{
		vec3f pos1 = first.GetTransform()[3].XYZ();

		float dist = Vec3::Dot(second.GetNormal(), pos1) - (first.GetRadius() - second.GetOffset());

		if (dist >= 0.0f)
		{
			return 0;
		}

		vec3f normal = second.GetNormal();

		Contact contact;
		contact.SetContactNormal(normal);
		contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
		contact.SetContactPoint(pos1 - second.GetNormal() * (first.GetRadius() + dist));
		contact.SetPenetration(-dist);
		contact.SetRigidbodies(first.GetBody(), NULL);
		contact.SetRestitution(data->Restitution);
		contact.SetFriction(data->Friction);

		data->AddContacts(contact);

		return 1;
	}

	uint32 SphereAndTruePlane(CollisionSphere &first, CollisionPlane &second, CollisionData *data)
	{
		vec3f pos1 = first.GetTransform()[3].XYZ();

		float dist = Vec3::Dot(second.GetNormal(), pos1) - (second.GetOffset());

		if (dist * dist >= first.GetRadius() * first.GetRadius())
		{
			return 0;
		}

		vec3f normal = second.GetNormal();
		float penetration = -dist;

		if (dist < 0)
		{
			normal *= -1.0f;
			penetration *= -1.0f;
		}

		Contact contact;
		contact.SetContactNormal(normal);
		contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
		contact.SetContactPoint(pos1 - second.GetNormal() * dist);
		contact.SetPenetration(penetration);
		contact.SetRigidbodies(first.GetBody(), NULL);
		contact.SetRestitution(data->Restitution);
		contact.SetFriction(data->Friction);

		data->AddContacts(contact);

		return 1;
	}

	void BoxAndHalfSpace(CollisionBox &first, CollisionPlane &second, CollisionData *data) 
	{
		vec3f vertices[8] =
		{
			vec3f(-first.GetHalfSize().X, -first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(-first.GetHalfSize().X, -first.GetHalfSize().Y, +first.GetHalfSize().Z),
			vec3f(-first.GetHalfSize().X, +first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(-first.GetHalfSize().X, +first.GetHalfSize().Y, +first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, -first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, -first.GetHalfSize().Y, +first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, +first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, +first.GetHalfSize().Y, +first.GetHalfSize().Z),
		};

		for (uint32 i = 0; i < 8; i++)
		{
			vertices[i] = first.GetTransform() * vertices[i];
		
			vec3f pos = vertices[i];

			float dist = Vec3::Dot(pos, second.GetNormal());

			if (dist <= second.GetOffset())
			{
				Contact contact;

				contact.SetContactNormal(second.GetNormal());
				contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
				contact.SetContactPoint((second.GetNormal() * (second.GetOffset() - dist)) + pos);
				contact.SetPenetration(second.GetOffset() - dist);
				contact.SetFriction(data->Friction);
				contact.SetRestitution(data->Restitution);
				contact.SetRigidbodies(first.GetBody(), NULL);

				data->AddContacts(contact);
			}
		}
	}

	void BoxAndTruePlane(CollisionBox &first, CollisionPlane &second, CollisionData *data)
	{
		vec3f vertices[8] =
		{
			vec3f(-first.GetHalfSize().X, -first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(-first.GetHalfSize().X, -first.GetHalfSize().Y, +first.GetHalfSize().Z),
			vec3f(-first.GetHalfSize().X, +first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(-first.GetHalfSize().X, +first.GetHalfSize().Y, +first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, -first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, -first.GetHalfSize().Y, +first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, +first.GetHalfSize().Y, -first.GetHalfSize().Z),
			vec3f(+first.GetHalfSize().X, +first.GetHalfSize().Y, +first.GetHalfSize().Z),
		};

		float rad = Vec3::LengthSquare(vertices[0] - vertices[7]);

		for (uint32 i = 0; i < 8; i++)
		{
			vertices[i] = first.GetTransform() * vertices[i];

			vec3f pos = vertices[i];

			float dist1 = Vec3::Dot(pos, 1.0f * second.GetNormal());
			float dist2 = Vec3::Dot(pos, -1.0f * second.GetNormal());

			if (dist1 <= second.GetOffset())
			{
				Contact contact;

				contact.SetContactNormal(second.GetNormal());
				contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
				contact.SetContactPoint((second.GetNormal() * (second.GetOffset() - dist1)) + pos);
				contact.SetPenetration(second.GetOffset() - dist1);
				contact.SetFriction(data->Friction);
				contact.SetRestitution(data->Restitution);
				contact.SetRigidbodies(first.GetBody(), NULL);

				data->AddContacts(contact);
			}
			else if (dist2 >= second.GetOffset())
			{
				Contact contact;

				contact.SetContactNormal(-1.0f*second.GetNormal());
				contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
				contact.SetContactPoint((-1.0f*second.GetNormal() * (-1.0f*second.GetOffset() - dist2)) + pos);
				contact.SetPenetration(second.GetOffset() - dist2);
				contact.SetFriction(data->Friction);
				contact.SetRestitution(data->Restitution);
				contact.SetRigidbodies(first.GetBody(), NULL);

				data->AddContacts(contact);
			}
		}
	}

	void BoxAndBox(CollisionBox &one, CollisionBox &two, CollisionData *data)
	{
		//if (!IntersectionTests::boxAndBox(one, two)) return 0;

		// Find the vector between the two centres
		vec3f toCentre = two.GetTransform()[3].XYZ() - one.GetTransform()[3].XYZ();

		// We start assuming there is no contact
		float pen = DBL_MAX;
		uint32 best = 8888888888;

		// Now we check each axes, returning if it gives us
		// a separating axis, and keeping track of the axis with
		// the smallest penetration otherwise.
		if (!TryAxis(one, two, one.GetTransform()[0].XYZ(), toCentre, (0), pen, best)) return;
		if (!TryAxis(one, two, one.GetTransform()[1].XYZ(), toCentre, (1), pen, best)) return;
		if (!TryAxis(one, two, one.GetTransform()[2].XYZ(), toCentre, (2), pen, best)) return;

		if (!TryAxis(one, two, two.GetTransform()[0].XYZ(), toCentre, (3), pen, best)) return;
		if (!TryAxis(one, two, two.GetTransform()[1].XYZ(), toCentre, (4), pen, best)) return;
		if (!TryAxis(one, two, two.GetTransform()[2].XYZ(), toCentre, (5), pen, best)) return;

		// Store the best axis-major, in case we run into almost
		// parallel edge collisions later
		unsigned bestSingleAxis = best;

		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[0].XYZ(), two.GetTransform()[0].XYZ()), toCentre, (6), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[0].XYZ(), two.GetTransform()[1].XYZ()), toCentre, (7), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[0].XYZ(), two.GetTransform()[2].XYZ()), toCentre, (8), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[1].XYZ(), two.GetTransform()[0].XYZ()), toCentre, (9), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[1].XYZ(), two.GetTransform()[1].XYZ()), toCentre, (10), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[1].XYZ(), two.GetTransform()[2].XYZ()), toCentre, (11), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[2].XYZ(), two.GetTransform()[0].XYZ()), toCentre, (12), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[2].XYZ(), two.GetTransform()[1].XYZ()), toCentre, (13), pen, best)) return;
		if (!TryAxis(one, two, Vec3::Cross(one.GetTransform()[2].XYZ(), two.GetTransform()[2].XYZ()), toCentre, (14), pen, best)) return;

		// We now know there's a collision, and we know which
		// of the axes gave the smallest penetration. We now
		// can deal with it in different ways depending on
		// the case.
		if (best < 3)
		{
			// We've got a vertex of box two on a face of box one.
			FillPointFaceBoxBox(one, two, toCentre, data, best, pen);
			return;
		}
		else if (best < 6)
		{
			// We've got a vertex of box one on a face of box two.
			// We use the same algorithm as above, but swap around
			// one and two (and therefore also the vector between their
			// centres).
			FillPointFaceBoxBox(two, one, toCentre*-1.0f, data, best - 3, pen);
			return;
		}
		else
		{
			// We've got an edge-edge contact. Find out which axes
			best -= 6;
			uint32 oneAxisIndex = best / 3;
			uint32 twoAxisIndex = best % 3;
			vec3f oneAxis = one.GetTransform()[oneAxisIndex].XYZ();
			vec3f twoAxis = two.GetTransform()[twoAxisIndex].XYZ();
			vec3f axis = Vec3::Cross(oneAxis, twoAxis);
			axis = Vec3::Normalize(axis);

			// The axis should point from box one to box two.
			if (Vec3::Dot(axis, toCentre) > 0) axis = axis * -1.0f;

			// We have the axes, but not the edges: each axis has 4 edges parallel
			// to it, we need to find which of the 4 for each object. We do
			// that by finding the point in the centre of the edge. We know
			// its component in the direction of the box's collision axis is zero
			// (its a mid-point) and we determine which of the extremes in each
			// of the other axes is closest.
			vec3f ptOnOneEdge = one.GetHalfSize();
			vec3f ptOnTwoEdge = two.GetHalfSize();
			for (unsigned i = 0; i < 3; i++)
			{
				if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
				else if (Vec3::Dot(one.GetTransform()[i].XYZ(), axis) > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

				if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
				else if (Vec3::Dot(two.GetTransform()[i].XYZ(), axis) < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
			}

			// Move them into world coordinates (they are already oriented
			// correctly, since they have been derived from the axes).
			ptOnOneEdge = one.GetTransform() * ptOnOneEdge;
			ptOnTwoEdge = two.GetTransform() * ptOnTwoEdge;

			// So we have a point and a direction for the colliding edges.
			// We need to find out point of closest approach of the two
			// line-segments.
			vec3f vertex = ContactPoint(
				ptOnOneEdge, oneAxis, one.GetHalfSize()[oneAxisIndex],
				ptOnTwoEdge, twoAxis, two.GetHalfSize()[twoAxisIndex],
				bestSingleAxis > 2
			);

			// We can fill the contact.
			Contact contact;
			
			contact.SetContactNormal(axis);
			contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
			contact.SetContactPoint(vertex);
			contact.SetPenetration(pen);
			contact.SetFriction(data->Friction);
			contact.SetRestitution(data->Restitution);
			contact.SetRigidbodies(one.GetBody(), two.GetBody());
			
			data->AddContacts(contact);
			
			return;
		}

		return;
	}

	void BoxAndSphere(CollisionBox &first, CollisionSphere &second, CollisionData *data) 
	{
		mat4f trans = second.GetTransform();
		vec3f center = trans[3].XYZ();

		vec3f relCenter = trans.Inverse() * center;

		if (fabsf(relCenter.X) - second.GetRadius() > first.GetHalfSize().X 
			|| fabsf(relCenter.Y) - second.GetRadius() > first.GetHalfSize().Y 
			|| fabsf(relCenter.Z) - second.GetRadius() > first.GetHalfSize().Z)
		{
			return;
		}

		vec3f closestPt;

		float dist;

		dist = relCenter.X;
		if (dist > first.GetHalfSize().X) dist = first.GetHalfSize().X;
		if (dist < -first.GetHalfSize().X) dist = -first.GetHalfSize().X;
		closestPt.X = dist;
	
		dist = relCenter.Y;
		if (dist > first.GetHalfSize().Y) dist = first.GetHalfSize().Y;
		if (dist < -first.GetHalfSize().Y) dist = -first.GetHalfSize().Y;
		closestPt.Y = dist;

		dist = relCenter.Z;
		if (dist > first.GetHalfSize().Z) dist = first.GetHalfSize().Z;
		if (dist < -first.GetHalfSize().Z) dist = -first.GetHalfSize().Z;
		closestPt.Z = dist;

		dist = Vec3::LengthSquare(closestPt - relCenter);

		if (dist > second.GetRadius() * second.GetRadius()) 
		{
			return;
		}

		vec3f closestPtW = first.GetTransform() * closestPt;

		Contact contact;

		contact.SetContactNormal(closestPtW - center);
		contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
		contact.SetContactPoint(closestPtW);
		contact.SetPenetration(second.GetRadius() - sqrtf(dist));
		contact.SetFriction(data->Friction);
		contact.SetRestitution(data->Restitution);
		contact.SetRigidbodies(first.GetBody(), second.GetBody());

		data->AddContacts(contact);
	}

private:
	
	float PenetrationOnAxis(CollisionBox &one, CollisionBox &two, vec3f &axis, vec3f &toCenter) 
	{
		float oneProject = TransformToAxis(one, axis);
		float twoProject = TransformToAxis(two, axis);

		float dist = fabsf(Vec3::Dot(axis, toCenter));

		return oneProject + twoProject - dist;
	}

	float TransformToAxis(CollisionBox &box, vec3f &axis) 
	{
		mat4f trans = box.GetTransform();
		
		float res = box.GetHalfSize().X * fabsf(Vec3::Dot(axis, trans[0].XYZ()));
		
		res += box.GetHalfSize().Y * fabsf(Vec3::Dot(axis, trans[1].XYZ()));
		
		res += box.GetHalfSize().Z * fabsf(Vec3::Dot(axis, trans[2].XYZ()));

		return res;
	}

	bool TryAxis(CollisionBox &one, CollisionBox &two, vec3f axis, vec3f &toCenter, uint32 index, float &smallestPenetration, uint32 &smallestIndex) 
	{
		if (Vec3::LengthSquare(axis) < 0.0001f)
		{
			return true;
		}

		axis = Vec3::Normalize(axis);

		float penetration = PenetrationOnAxis(one, two, axis, toCenter);

		if (penetration < 0)
		{
			return false;
		}

		if (penetration < smallestPenetration)
		{
			smallestPenetration = penetration;
			smallestIndex = index;
		}

		return true;
	}
	
	void FillPointFaceBoxBox(
		CollisionBox &one,
		CollisionBox &two,
		vec3f &toCentre,
		CollisionData *data,
		uint32 best,
		float pen
	)
	{
		// This method is called when we know that a vertex from
		// box two is in contact with box one.

		Contact contact;

		// We know which axis the collision is on (i.e. best),
		// but we need to work out which of the two faces on
		// this axis.
		vec3f normal = one.GetTransform()[best].XYZ();
		if (Vec3::Dot(normal, toCentre) > 0)
		{
			normal = normal * -1.0f;
		}

		// Work out which vertex of box two we're colliding with.
		// Using toCentre doesn't work!
		vec3f vertex = two.GetHalfSize();
		if (Vec3::Dot(two.GetTransform()[0].XYZ(), normal) < 0) vertex.X = -vertex.X;
		if (Vec3::Dot(two.GetTransform()[1].XYZ(), normal) < 0) vertex.Y = -vertex.Y;
		if (Vec3::Dot(two.GetTransform()[2].XYZ(), normal) < 0) vertex.Z = -vertex.Z;

		// Create the contact data
		contact.SetContactNormal(normal);
		contact.SetContactNormal(Vec3::Normalize(contact.GetContactNormal()));
		contact.SetPenetration(pen);
		contact.SetContactPoint(two.GetTransform() * vertex);
		contact.SetRigidbodies(one.GetBody(), two.GetBody());
		contact.SetFriction(data->Friction);
		contact.SetRestitution(data->Restitution);

		data->AddContacts(contact);
	}

	vec3f ContactPoint(
		vec3f &pOne,
		vec3f &dOne,
		float oneSize,
		vec3f &pTwo,
		vec3f &dTwo,
		float twoSize,

		// If this is true, and the contact point is outside
		// the edge (in the case of an edge-face contact) then
		// we use one's midpoint, otherwise we use two's.
		bool useOne)
	{
		vec3f toSt, cOne, cTwo;
		float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
		float denom, mua, mub;

		smOne = Vec3::LengthSquare(dOne);
		smTwo = Vec3::LengthSquare(dTwo);
		dpOneTwo = Vec3::Dot(dTwo, dOne);

		toSt = pOne - pTwo;
		dpStaOne = Vec3::Dot(dOne, toSt);
		dpStaTwo = Vec3::Dot(dTwo, toSt);

		denom = smOne * smTwo - dpOneTwo * dpOneTwo;

		// Zero denominator indicates parrallel lines
		if (fabsf(denom) < 0.0001f) {
			return useOne ? pOne : pTwo;
		}

		mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
		mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

		// If either of the edges has the nearest point out
		// of bounds, then the edges aren't crossed, we have
		// an edge-face contact. Our point is on the edge, which
		// we know from the useOne parameter.
		if (mua > oneSize ||
			mua < -oneSize ||
			mub > twoSize ||
			mub < -twoSize)
		{
			return useOne ? pOne : pTwo;
		}
		else
		{
			cOne = pOne + dOne * mua;
			cTwo = pTwo + dTwo * mub;

			return cOne * 0.5f + cTwo * 0.5f;
		}
	}
};
