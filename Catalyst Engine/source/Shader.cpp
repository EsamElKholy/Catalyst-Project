#include <Shader.h>

bool Shader::CreateShaders()
{
	Unload();

	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	unsigned int geometryShader = 0;

	if (VertexPath && FragmentPath)
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		char *vertexData = LoadTextFile(VertexPath);
		char *fragmentData = LoadTextFile(FragmentPath);

		glShaderSource(vertexShader, 1, &vertexData, 0);
		glShaderSource(fragmentShader, 1, &fragmentData, 0);

		{
			glCompileShader(vertexShader);

			delete vertexData;
			vertexData = NULL;

			int success;
			char log[512];

			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, 0, log);

				std::cout << "Couldn't compile vertex shader\n";
				std::cout << "Error log: \n" << log << std::endl;
				return false;
			}
		}

		{
			glCompileShader(fragmentShader);

			delete fragmentData;
			fragmentData = NULL;

			int success;
			char log[512];

			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(fragmentShader, 512, 0, log);

				std::cout << "Couldn't compile fragment shader\n";
				std::cout << "Error log: \n" << log << std::endl;
				return false;
			}
		}
	}
	else
	{
		if (!VertexPath)
		{
			std::cout << "Invalid vertex shader path\n";
		}

		if (!FragmentPath)
		{
			std::cout << "Invalid fragment shader path\n";
		}

		return false;
	}

	if (GeometryPath)
	{
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		char *geometryData = LoadTextFile(GeometryPath);

		glShaderSource(geometryShader, 1, &geometryData, 0);

		glCompileShader(geometryShader);

		delete geometryData;
		geometryData = NULL;

		int success;
		char log[512];

		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(geometryShader, 512, 0, log);

			std::cout << "Couldn't compile geometry shader\n";
			std::cout << "Error log: \n" << log << std::endl;
		}
	}
	else
	{
		std::cout << "Invalid geometry shader path\n";
	}

	Program = glCreateProgram();

	if (vertexShader)
	{
		glAttachShader(Program, vertexShader);
		glDeleteShader(vertexShader);
	}

	if (fragmentShader)
	{
		glAttachShader(Program, fragmentShader);
		glDeleteShader(fragmentShader);
	}

	if (geometryShader)
	{
		glAttachShader(Program, geometryShader);
		glDeleteShader(geometryShader);
	}

	glLinkProgram(Program);

	int success;
	char log[512];
	GLenum e = glGetError();
	glGetProgramiv(Program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(Program, 512, 0, log);

		std::cout << "Couldn't link geometry shader\n";
		std::cout << "Error log: \n" << log << std::endl;

		return false;
	}

	return true;
}

void Shader::Activate()
{
	glUseProgram(Program);
}

void Shader::Deactivate()
{
	glUseProgram(NULL);
}

bool Shader::IsAvailable()
{
	if (Program)
	{
		return true;
	}

	return false;
}

void Shader::Unload()
{
	if (Program)
	{
		glDeleteProgram(Program);
		Program = 0;
	}
	else
	{
		Program = 0;
	}
}

void Shader::AddUniform(const std::string &name)
{
	int32 loc = GetUniform(name);

	if (loc < 0)
	{
		const char *nc = name.c_str();
		Uniforms[name] = glGetUniformLocation(Program, nc);
	}
}

int32 Shader::GetUniform(const std::string &name)
{
	std::map<std::string, int32>::iterator res = Uniforms.find(name);

	if (res != Uniforms.end())
	{
		return res->second;
	}
	else
	{
		return -1;
	}
}

void Shader::SetUniform1i(const std::string &name, const int &v)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform1i(loc, v);
	}
}

void Shader::SetUniform1f(const std::string &name, const float &v)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform1f(loc, v);
	}
}

void Shader::SetUniform2f(const std::string &name, const float &v1, const float &v2)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform2f(loc, v1, v2);
	}
}

void Shader::SetUniform2f(const std::string &name, const vec2f &v)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform2f(loc, v.X, v.Y);
	}
}

void Shader::SetUniform3f(const std::string &name, const float &v1, const float &v2, const float &v3)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform3f(loc, v1, v2, v3);
	}
}

void Shader::SetUniform3f(const std::string &name, const vec3f &v)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform3f(loc, v.X, v.Y, v.Z);
	}
}

void Shader::SetUniform4f(const std::string &name, const float &v1, const float &v2, const float &v3, const float &v4)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform4f(loc, v1, v2, v3, v4);
	}
}

void Shader::SetUniform4f(const std::string &name, const vec4f &v)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform4f(loc, v.X, v.Y, v.Z, v.W);
	}
}

void Shader::SetUniform4f(const std::string &name, const quatf &v)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniform4f(loc, v.X, v.Y, v.Z, v.W);
	}
}

void Shader::SetUniformMat4(const std::string &name, const mat4f &v)
{
	int32 loc = GetUniform(name);

	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, 0, v.Elements_1D);
	}
}

void Shader::AddAttribute(const std::string &name, const int32 &index)
{
	int32 loc = GetAttribute(name);

	if (loc < 0)
	{
		glBindAttribLocation(Program, index, name.c_str());
		Attributes[name] = glGetAttribLocation(Program, name.c_str());
	}
}

int32 Shader::GetAttribute(const std::string &name)
{
	std::map<std::string, int32>::iterator res = Attributes.find(name);

	if (res != Attributes.end())
	{
		return res->second;
	}
	else
	{
		return -1;
	}
}