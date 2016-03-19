#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct Light {
    vec3 Position;
    vec3 Color;
    float Constant;
    float Linear;
    float Quadratic;
};
uniform Light light;

void main() {
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    vec3 ambient = vec3(0.3f * AmbientOcclusion);
    vec3 lighting = ambient; 
    vec3 viewDir = normalize(-FragPos); // Viewpos is (0.0.0)
	
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
	
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0f), 8.0f);
    vec3 specular = light.Color * spec;
	
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
	
    lighting += diffuse + specular;

    FragColor = vec4(lighting, 1.0);
	
	//FragColor = vec4(vec3(AmbientOcclusion), 1.0);
	
	//FragColor = vec4(vec3((50.0f - texture(gPositionDepth, TexCoords).w) / 50.0f), 1.0);
}