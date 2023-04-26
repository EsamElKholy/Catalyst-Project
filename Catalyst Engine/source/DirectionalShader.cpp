#include <DirectionalShader.h>

void DirectionalShader::UpdateUniforms(mat4f &m, Camera &cam, vec3f &c)
{
	Activate();

	SetUniformMat4("model", m);
	SetUniformMat4("view", cam.GetViewMatrix());
	SetUniformMat4("projection", cam.GetProjectionMatrix());

	SetUniform3f("color", c);

	SetUniform3f("eyePos", cam.GetActiveView().Position);
	vec3f dir(-12, -10, -10);
	vec3f diff(1, 1, 1);
	vec3f spec(1, 1, 1);
	vec3f amb(1, 1, 1);
	float ambstr = 0.7f;
	SetUniform3f("dirLight.direction", dir);
	SetUniform3f("dirLight.ambient", amb);
	SetUniform1f("dirLight.ambientStrength", ambstr);
	SetUniform3f("dirLight.diffuse", diff);
	SetUniform3f("dirLight.specular", spec);
}