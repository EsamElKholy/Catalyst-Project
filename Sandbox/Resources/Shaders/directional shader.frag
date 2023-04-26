#version 330

in vec3 frag_normal;
in vec3 frag_eyePos;
in vec3 frag_pos;
in vec2 frag_texcoords;

out vec4 OutColor;

struct DirectionalLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float ambientStrength;
};

uniform vec3 color;
uniform DirectionalLight dirLight;

vec3 calcDirectional(DirectionalLight dir, vec3 normal, vec3 viewDir)
{
// Ambient part
	vec3 ambCol = dir.ambient * dir.ambientStrength * color;

// Diffuse part
	vec3 norm = (normal);
	vec3 lightDir = normalize(-dir.direction);

	float diffuseAngle = max(dot(norm, lightDir), 0.0f);
	vec3 diffCol = diffuseAngle * dir.diffuse * color;
	
// Specular part
	vec3 halfway = normalize(viewDir + lightDir);
	//vec3 specRefl = reflect(-lightDir, norm);

	float spec = max(pow(dot(norm, halfway), 64.0f), 0.0f);
	//float spec = pow(max(dot(norm, specRefl), 0.0f), 32.0f);
	vec3 specCol = spec * dir.specular * 5.0f * color;

	vec3 finalLight = ambCol + (diffCol + specCol);

	return finalLight;
}

uniform sampler2D myTex;

void main()
{
	vec4 tcol = texture(myTex, frag_texcoords);
	vec4 col = vec4(1);

	vec3 viewDir = normalize(frag_eyePos - frag_pos);
	vec3 normal = normalize(frag_normal);

	vec3 dirResult = calcDirectional(dirLight, normal, viewDir);
	
	OutColor = vec4(dirResult, 1) * vec4(color, 1) * tcol;	   	
	//OutColor = vec4(color, 1);	   	
}