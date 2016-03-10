#version 330 core
in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;
  
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
}; 

struct Light {
    vec3 position;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main() {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords))); 
	
	vec3 emission = vec3(texture(material.emission, TexCoords));
	
    color = vec4(ambient + diffuse + specular + emission, 1.0f);
}