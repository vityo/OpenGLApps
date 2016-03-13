#version 330 core
in vec2 TexCoords;

out vec4 color;
  
uniform sampler2D screenTexture;

void main() {
	if(gl_FragCoord.x > 325) {
		discard;
	}
	else {
        color = texture(screenTexture, TexCoords);
	}
}