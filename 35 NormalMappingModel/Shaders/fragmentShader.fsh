#version 330 core
in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

in vec3 Normal;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	vec3 texture_diffuse_color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
	vec3 ambient = vec3(0.15f) * texture_diffuse_color;
  	
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
	if (gl_FragCoord.x > 400) {
		normal = texture(texture_normal1, fs_in.TexCoords).rgb;
		normal = normalize(normal * 2.0f - 1.0f);
		
		lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
		viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	}
	else {		
		normal = normalize(Normal);
		lightDir = normalize(lightPos - fs_in.TangentFragPos);
		viewDir = normalize(viewPos - fs_in.TangentFragPos);
	}
	
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = vec3(1.0f) * diff * texture_diffuse_color;
    
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
	
    vec3 specular = vec3(0.5f) * spec * texture(texture_specular1, fs_in.TexCoords).rgb; 
	
	color = vec4(ambient + diffuse + specular, 1.0f);
}