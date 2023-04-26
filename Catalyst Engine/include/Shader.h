#pragma once

#include <map>
#include <GL\glew.h>

#include "Utilities.h"
#include <cinder\CinderMath.h>

using namespace Cinder::Math;

class Shader
{
public:
	Shader(char *vertex = NULL, char *fragment = NULL, char *geometry = NULL)
		: VertexPath(vertex), FragmentPath(fragment), GeometryPath(geometry)
	{
		Unload();
	}

	virtual ~Shader()
	{
		Unload();
	}

public:
	bool CreateShaders();
	void Activate();
	void Deactivate();
	bool IsAvailable();
	void Unload();

	void AddUniform(  const std::string &name);
	int32 GetUniform( const std::string &name);
					  
	void SetUniform1i(const std::string &name, const int &v);
	void SetUniform1f(const std::string &name, const float &v);

	void SetUniform2f(const std::string &name, const float &v1, const float &v2);
	void SetUniform2f(const std::string &name, const vec2f &v);
					  
	void SetUniform3f(const std::string &name, const float &v1, const float &v2, const float &v3);
	void SetUniform3f(const std::string &name, const vec3f &v);
					 
	void SetUniform4f(const std::string &name, const float &v1, const float &v2, const float &v3, const float &v4);
	void SetUniform4f(const std::string &name, const vec4f &v);
	void SetUniform4f(const std::string &name, const quatf &v);

	void SetUniformMat4(const std::string &name, const mat4f &v);

	void AddAttribute(const std::string &name, const int32 &index);
	int32 GetAttribute(const std::string &name);

protected:
	uint32 Program;

private:
	std::map<std::string, int32> Uniforms;
	std::map<std::string, int32> Attributes;


	char *VertexPath;
	char *FragmentPath;
	char *GeometryPath;
};
