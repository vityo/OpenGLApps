#version 330 core
in VS_OUT {
    vec2 texCoords;
} fs_in; 

out vec4 color;

uniform sampler2D texture_diffuse1;

void main() {
	color = texture(texture_diffuse1, fs_in.texCoords);   
}