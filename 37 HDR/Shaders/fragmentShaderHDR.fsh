#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;
uniform bool gamma;

void main() {
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	vec3 result;
	
	if(gl_FragCoord.x < 400) {
		// without
		result = hdrColor;
	}
	else {
		// reinhard
		//vec3 result = hdrColor / (hdrColor + vec3(1.0f));
		// exposure
		result = vec3(1.0f) - exp(-hdrColor * exposure);
	}
	
	// gamma correct 
	if(gamma) {
		const float gamma = 2.2f;    
		result = pow(result, vec3(1.0f / gamma));
	}
	
	color = vec4(result, 1.0f);
}