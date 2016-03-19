#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
struct Light {
    vec3 Position;
    vec3 Color;
    float Radius;
};
const int NR_LIGHTS = 50;
const float constant = 1.0f;
const float linear = 0.7f;
const float quadratic = 1.8f;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main() {
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
	
	vec3 lighting = Albedo * 0.1f;
    vec3 viewDir = normalize(viewPos - FragPos);
	
    for(int i = 0; i < NR_LIGHTS; ++i) {
		float distance = length(lights[i].Position - FragPos);
		
        if(distance < lights[i].Radius) {
			vec3 lightDir = normalize(lights[i].Position - FragPos);
			float diff = max(dot(Normal, lightDir), 0.0f);
			vec3 diffuse = lights[i].Color * Albedo * diff;
			
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 halfwayDir = normalize(lightDir + viewDir);  
			float spec = pow(max(dot(Normal, halfwayDir), 0.0f), 64.0f);
			vec3 specular = lights[i].Color * Specular * spec; 
			
			float attenuation = 1.0f / (constant + linear * distance + quadratic * distance * distance);
	
			diffuse *= attenuation;
			specular *= attenuation; 
	
			lighting += diffuse + specular;
		}
    }
    
    FragColor = vec4(lighting, 1.0);
}