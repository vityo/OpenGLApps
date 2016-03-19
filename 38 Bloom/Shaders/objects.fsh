#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
	vec3 FragPos;  
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D emission;

struct Light {
	vec3 Position;
	vec3 Color;
};
uniform	Light lights[4];
uniform vec3 viewPos;

void main() {
	vec3 result;
	vec3 texture_diffuse_color = texture(diffuse, fs_in.TexCoords).rgb;
	vec3 texture_specular_color = texture(specular, fs_in.TexCoords).rgb;
	vec3 texture_emission_color = texture(emission, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	
	for(int i = 0; i < 4; ++i) {
		vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
		
		float diff = max(dot(normal, lightDir), 0.0);
		
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
		
		float distance = length(lightDir);
		float attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * (distance * distance));
		
		vec3 ambient  = vec3(0.15f) * texture_diffuse_color;
		vec3 diffuse  = lights[i].Color  * diff * texture_diffuse_color;
		vec3 specular = vec3(3.0f) * lights[i].Color * spec * texture_specular_color;
		ambient  *= attenuation;
		diffuse  *= attenuation;
		specular *= attenuation;
		
		result += ambient + diffuse + specular;
	}
	
	result += 2.0f * texture_emission_color;
	
	FragColor = vec4(result, 1.0f);
	
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) {
        BrightColor = vec4(result, 1.0);
	}
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}