#version 330 core
out vec4 color;
  
uniform vec3 objectColor;

void main() {
	if(gl_FragCoord.x < 475) {
        color = vec4(vec3(1.0f) - objectColor, 1.0f);
	}
    else {
		color = vec4(objectColor, 1.0f);
	}
}