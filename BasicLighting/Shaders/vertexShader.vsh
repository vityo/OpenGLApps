#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec4 myColor;

uniform vec3 lightPos;  
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vec3 FragPos = vec3(view * model * vec4(position, 1.0f));
    vec3 Normal = mat3(transpose(inverse(view * model))) * normal;
	vec3 LightPos = vec3(view * vec4(lightPos, 1.0f));
	
	float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * objectColor;
    myColor = vec4(result, 1.0f);
} 