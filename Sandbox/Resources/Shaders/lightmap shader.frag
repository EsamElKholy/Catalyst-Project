#version 330

in vec2 frag_texcoord;
in vec3 frag_normal;
in vec3 frag_pos;
in vec3 frag_eyePos;

out vec4 out_color;

struct Material
{
	sampler2D Diffuse;
	sampler2D Specular;

	float Shininess;
};

struct DirectionalLight
{
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	float AmbientStrength;
};

struct Attenuation 
{
	float Constant;
	float Linear;
	float Quadratic;
};

struct PointLight 
{
	vec3 Position;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	Attenuation Atten;

	float AmbientStrength;
};

struct SpotLight
{
	vec3 Position;
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	Attenuation Atten;

	float AmbientStrength;

	float InnerCut;
	float OuterCut;
};

uniform Material objMaterial;
uniform vec3 objectColor;

uniform DirectionalLight directional;

#define NUM_POINT_LIGHT 10
//uniform PointLight point;
uniform PointLight points[NUM_POINT_LIGHT];
uniform float MAX_POINT_LIGHT_TO_USE;

//uniform SpotLight spot;

vec3 calcDirectional(DirectionalLight dir, vec3 normal, vec3 viewDir)
{
	vec3 ambCol = dir.Ambient * dir.AmbientStrength * texture(objMaterial.Diffuse, frag_texcoord).xyz;

	vec3 norm = (normal);
	vec3 lightDir = normalize(-dir.Direction);

	float diffuseAngle = max(dot(norm, lightDir), 0.0f);
	vec3 diffCol = diffuseAngle * dir.Diffuse * texture(objMaterial.Diffuse, frag_texcoord).xyz;

	//vec3 reflectionDir = normalize(reflect(-lightDir, norm));
	vec3 halfway = normalize(viewDir + lightDir);
	
	float spec = pow(max(dot(norm, halfway), 0.0f), 32.0f);
	vec3 specCol = spec * dir.Specular * objMaterial.Shininess * texture(objMaterial.Specular, frag_texcoord).xyz;

	vec3 finalLight = ambCol + (diffCol + specCol);

	return finalLight;
}

vec3 calcPoint(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 ambCol = pointLight.Ambient * pointLight.AmbientStrength * texture(objMaterial.Diffuse, frag_texcoord).xyz;

	vec3 norm = (normal);
	vec3 lightDir = normalize(pointLight.Position - fragPos);

	float diffuseAngle = max(dot(norm, lightDir), 0.0f);
	vec3 diffCol = diffuseAngle * pointLight.Diffuse * texture(objMaterial.Diffuse, frag_texcoord).xyz;

	//vec3 reflectionDir = normalize(reflect(-lightDir, norm));
	vec3 halfway = normalize(viewDir + lightDir);
	
	float spec = pow(max(dot(norm, halfway), 0.0f), 64.0f);
	vec3 specCol = spec * pointLight.Specular * objMaterial.Shininess * texture(objMaterial.Specular, frag_texcoord).xyz;
	
	float dist = length(pointLight.Position - fragPos);
	float atten = 1.0f / (pointLight.Atten.Constant
						 + (pointLight.Atten.Linear * dist)
						 + (pointLight.Atten.Quadratic * pow(dist, 2.0f)));

	vec3 finalLight = (atten * ambCol) + (atten * diffCol) + (atten * specCol);

	return finalLight;
}

vec3 calcSpot(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 ambCol = spotLight.Ambient * spotLight.AmbientStrength * texture(objMaterial.Diffuse, frag_texcoord).xyz;

	vec3 norm = (normal);
	vec3 lightDir = normalize(spotLight.Position - frag_pos);

	float diffuseAngle = max(dot(norm, lightDir), 0.0f);
	vec3 diffCol = diffuseAngle * spotLight.Diffuse * texture(objMaterial.Diffuse, frag_texcoord).xyz;

	//vec3 reflectionDir = normalize(reflect(-lightDir, norm));
	vec3 halfway = normalize(viewDir + lightDir);
	
	float spec = pow(max(dot(norm, halfway), 0.0f), 32.0f);
	vec3 specCol = spec * spotLight.Specular * objMaterial.Shininess * texture(objMaterial.Specular, frag_texcoord).xyz;
	
	float dist = length(spotLight.Position - fragPos);
	float atten = 1.0f / (spotLight.Atten.Constant
						 + (spotLight.Atten.Linear * dist)
						 + (spotLight.Atten.Quadratic * pow(dist, 2.0f)));

	float theta = dot(lightDir, normalize(-spotLight.Direction));
	float epsilon = spotLight.InnerCut - spotLight.OuterCut;
	float intensity = clamp((theta - spotLight.OuterCut) / epsilon, 0, 1);

	vec3 finalLight = atten * ambCol;
	
	finalLight += (atten * intensity * diffCol) + (atten * intensity * specCol);
	
	return finalLight;
}

void main()
{
	vec3 viewDir = normalize(frag_eyePos - frag_pos);
	vec3 normal = normalize(frag_normal);
	
	vec3 dirResult = calcDirectional(directional, normal, viewDir);
	vec3 pointResult = vec3(0, 0, 0);//calcPoint(point, normal, frag_pos, viewDir);
	//vec3 spotResult = calcSpot(spot, normal, frag_pos, viewDir);

	int maxPoint = 0;

	if (MAX_POINT_LIGHT_TO_USE < 0)
	{
		maxPoint = 0;
	}	
	else if (MAX_POINT_LIGHT_TO_USE > NUM_POINT_LIGHT)
	{
		maxPoint = NUM_POINT_LIGHT;
	}
	else
	{
		maxPoint = int(MAX_POINT_LIGHT_TO_USE);
	}
	
	for(int i = 0; i < maxPoint; i++)
	{
		pointResult += calcPoint(points[i], normal, frag_pos, viewDir);
	}

	//float dMax = 50;
	//float dMin = 25;
	//float dist = length(frag_eyePos - frag_pos);
	//float fog = (dMax - dist) / (dMax - dMin);
	//fog = clamp(fog, 0, 1);

	vec3 finalLight = dirResult + pointResult; /*+ spotResult;*/
	//vec3 color = mix(vec3(0.15f, 0.15f, 0.15f), (finalLight) * (objectColor), fog);	
	out_color = vec4(finalLight * objectColor, 1);
}