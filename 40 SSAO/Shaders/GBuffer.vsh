#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	mat4 viewModel = view * model;
	vec4 viewPos = viewModel * vec4(position, 1.0f);
	FragPos = viewPos.xyz;
    gl_Position = projection * viewPos;
    TexCoords = texCoords;
    mat3 normalMatrix = transpose(inverse(mat3(viewModel)));
    Normal = normalMatrix * normal;
}  