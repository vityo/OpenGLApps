#version 330 core
in vec2 TexCoords;

out vec4 color;
  
uniform sampler2D texture1;

float near = 0.1; 
float far  = 20.0; 
  
float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
	if(gl_FrontFacing) {
		float depth = LinearizeDepth(gl_FragCoord.z) / far;
		color = vec4(vec3(depth), 1.0f);
	}
	else {
		color = texture(texture1, TexCoords);
	}
}