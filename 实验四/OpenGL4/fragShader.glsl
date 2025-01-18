#version 430

in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingNormal;
in vec3 varyingTangent;
in vec3 originalVertex;
in vec2 tc;
in vec3 varyingHalfVector;

out vec4 fragColor;

layout (binding=0) uniform sampler2D normMap;
layout (binding = 1) uniform sampler2D colorMap; // �����ɫ����
uniform bool useLight;  // �����Ƿ�ʹ�ù���

struct PositionalLight
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	vec3 position;
};

struct Material
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;	 
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

vec3 calcNewNormal()
{
	vec3 normal = normalize(varyingNormal);
	vec3 tangent = normalize(varyingTangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(tangent, normal);
	mat3 tbn = mat3(tangent, bitangent, normal);
	vec3 retrievedNormal = texture(normMap,tc).xyz;
	retrievedNormal = retrievedNormal * 2.0 - 1.0;
	vec3 newNormal = tbn * retrievedNormal;
	newNormal = normalize(newNormal);
	return newNormal;
}

void main(void)
{
    // ��ȡ������ɫ
    vec4 texColor = texture(colorMap, tc);
    if (useLight) {
        vec3 L = normalize(varyingLightDir);
        vec3 N = normalize(varyingNormal);
        vec3 H = normalize(varyingHalfVector);
        // ������սǶ�
        float cosTheta = dot(L, N);
        float cosPhi = dot(H, N);
        // ���������ɫ
        vec4 lightColor = globalAmbient * material.ambient
            + light.ambient * material.ambient
            + light.diffuse * material.diffuse * max(cosTheta, 0.0)
            + light.specular * material.specular * pow(max(cosPhi, 0.0), material.shininess * 3.0);
        // ���������ɫ�͹���
        fragColor = lightColor * texColor;
    }
    else {
        // ֱ��ʹ��������ɫ
        fragColor = texColor;
    }
}
