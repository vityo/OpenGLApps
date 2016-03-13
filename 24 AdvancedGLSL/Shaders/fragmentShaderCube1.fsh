#version 330 core
in vec2 TexCoords;

out vec4 color;
  
uniform sampler2D texture1;

void main() {
	if(mod(gl_FragCoord.x, 100) < 60 && mod(gl_FragCoord.y, 100) < 60) {
		color = texture(texture1, TexCoords);
	}
	else {
		color = vec4(0.7f) - texture(texture1, TexCoords);
	}
}