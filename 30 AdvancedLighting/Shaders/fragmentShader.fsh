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

void main() {
    vec3 ambient = light.ambient * vec3(texture(diffuse, TexCoords));
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(diffuse, TexCoords)));
    
	float spec = 0.0; 
	vec3 viewDir = normalize(viewPos - FragPos);
	
    if(gl_FragCoord.x < 400) {
        vec3 reflectDir = reflect(-lightDir, norm);  
		spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.8f);
    }
    else {
	// blinn-phong
		vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(norm, halfwayDir), 0.0), 1.6f);
    }
	
    vec3 specular = light.specular * spec; 
	
    color = vec4(ambient + diffuse + specular, 1.0f);
}