#version 330 core
in vec3 Normal;
in vec3 Position;

out vec4 color;
 
uniform vec3 cameraPos;
uniform samplerCube skyBox;

void main() {
	float ratio = 1.0f / 1.52f;
	vec3 I = normalize(Position - cameraPos);
	vec3 R = refract(I, normalize(Normal), ratio);
    color = texture(skyBox, R);
}