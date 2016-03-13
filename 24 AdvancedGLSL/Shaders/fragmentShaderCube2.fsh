#version 420 core
in vec2 TexCoords;

layout (depth_less) out float gl_FragDepth;
out vec4 color;
  
uniform sampler2D texture1;

void main() {
	gl_FragDepth = gl_FragDepth * 0.1f;
	color = texture(texture1, TexCoords);
}