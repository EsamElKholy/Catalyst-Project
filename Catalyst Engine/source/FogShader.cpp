#include <FogShader.h>
#include <Camera.h>

void FogShader::UpdateUniforms(mat4f & m, Camera * cam, vec3f & c, vec3f & fogCol, float start, float end, float fogP)
{
	Activate();

	SetUniformMat4("model", m);
	SetUniformMat4("view", cam->GetViewMatrix());
	SetUniformMat4("projection", cam->GetProjectionMatrix());

	SetUniform3f("color", c);

	SetUniform3f("eyePos", cam->GetPosition());

	SetUniform3f("fogColor", fogCol);
	SetUniform1f("start", start);
	SetUniform1f("end", end);
	SetUniform1f("fogP", fogP);
}
