#version 330 core
layout (location = 0) in vec3 position;
out vec4 vertexColor;
uniform vec2 shift;

void main() {
	gl_Position = vec4(position.x + shift.x, position.y + shift.y, position.z, 1.0f);
	vertexColor = vec4(shift.x, shift.y, 0.0f, 1.0f); 
}