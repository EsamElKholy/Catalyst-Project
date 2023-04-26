#include <BasicShader.h>
#include <Camera.h>

void BasicShader::UpdateUniforms(mat4f & m, Camera *cam, vec3f & c)
{
	Activate();

	SetUniformMat4("model", m);
	SetUniformMat4("view", cam->GetViewMatrix());
	SetUniformMat4("projection", cam->GetProjectionMatrix());

	SetUniform3f("color", c);
}
