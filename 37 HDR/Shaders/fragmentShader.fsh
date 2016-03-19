#version 330 core

in VS_OUT {
	vec3 FragPos;  
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec3 viewPos;
struct Light {
	vec3 Position;
	vec3 Color;
};
uniform	Light lights[4];

void main() {
	vec3 result;
	vec3 texture_color = texture(diffuse, fs_in.TexCoords).rgb;
	
	for(int i = 0; i < 4; ++i) {
		vec3 normal = normalize(fs_in.Normal);
		vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = lights[i].Color * diff * texture_color;
		
		vec3 color = diffuse;
		float distance = length(fs_in.FragPos - lights[i].Position);
		color *= 1.0 / (1.0f + 0.09f * distance + 0.032f * distance * distance);
		
		result += color;
	}
	
	FragColor = vec4(result, 1.0f);
}