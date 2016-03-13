#version 330 core
out vec4 color;
  
uniform vec3 objectColor;

void main() {
	if(gl_FragCoord.x > 325) {
		discard;
	}
	else {
        color = vec4(1.0f);
	}
}