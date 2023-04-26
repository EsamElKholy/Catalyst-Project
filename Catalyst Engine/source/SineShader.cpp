#include <SineShader.h>
#include <Camera.h>

void SineShader::UpdateUniforms(mat4f & m, Camera *cam, vec3f & c, float t)
{
	Activate();

	SetUniformMat4("model", m);
	SetUniformMat4("view", cam->GetViewMatrix());
	SetUniformMat4("projection", cam->GetProjectionMatrix());

	SetUniform3f("color", c);
	SetUniform1f("increament", t);
}
