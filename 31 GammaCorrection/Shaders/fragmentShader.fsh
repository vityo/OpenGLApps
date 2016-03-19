#version 330 core
in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;
  
uniform vec3 viewPos;
uniform sampler2D diffuse;

struct Light {
    vec3 position;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform bool gamma;

void main() {
    vec3 ambient = light.ambient * vec3(texture(diffuse, TexCoords));
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(diffuse, TexCoords)));
    
	float spec = 0.0; 
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	spec = pow(max(dot(norm, halfwayDir), 0.0), 1.6f);
	
    vec3 specular = light.specular * spec; 
	
	vec3 colorWithGamma = ambient + diffuse + specular;
	
	if(gamma) {
        colorWithGamma = pow(colorWithGamma, vec3(1.0/2.2));
	}
		
    color = vec4(colorWithGamma, 1.0f);
}