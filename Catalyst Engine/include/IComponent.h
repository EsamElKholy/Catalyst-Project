#pragma once

class IActor;

class IComponent
{
public:
	IComponent(std::string name = "", std::string type = "NOT_SPECIFIED")
		: Name(name), Type(type), Owner(0)
	{}

	virtual ~IComponent() {}

public:
	// NOTE(Kai): Pure virtual methods
	virtual void Init() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void Destroy() = 0;

public:
	// NOTE(Kai): Getters and Setters

	inline IActor* GetOwner() { return Owner; }
	inline std::string GetName() { return Name; }
	inline std::string GetType() { return Type; }
	inline bool IsEnabled() { return Enable; }

	inline void SetOwner(IActor *owner) { Owner = owner; }
	inline void SetName(const std::string &name) { Name = name; }
	inline void SetType(const std::string &type) { Type = type; }
	inline void SetEnable(bool e) { Enable = e; }
protected:
	IActor *Owner;
	std::string Name;
	std::string Type;
	bool Enable;
};
