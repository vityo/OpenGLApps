#version 330 core
in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_reflect1;
uniform vec3 cameraPos;
uniform samplerCube skyBox;

void main() {
	vec4 diffuse_color = texture(texture_diffuse1, TexCoords);
	
	vec4 reflect_intensity = texture(texture_reflect1, TexCoords);
	vec3 I = normalize(Position - cameraPos);
	vec3 R = reflect(I, normalize(Normal));
	vec4 reflect_color = texture(skyBox, R) * reflect_intensity;

	// data missing
	//color = diffuse_color + reflect_color;
	
	color = vec4(0.0f, 0.0f, 0.1f, 1.0f) + reflect_color;
}