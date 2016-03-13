#version 330 core
in vec2 TexCoords;

out vec4 color;
  
uniform sampler2D screenTexture;

void main() {
// normal
//	color = texture(screenTexture, TexCoords);
// inversion
//	color = vec4(vec4(1.0f) - texture(screenTexture, TexCoords));
// grayscale
/*	color = texture(screenTexture, TexCoords);
	// обычный
	// float average = (color.r + color.g + color.b) / 3.0;
	// под глаза человека
	float average = 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b; 
	
	color = vec4(average, average, average, 1.0);
*/
	if(gl_FragCoord.x < 325) {
		discard;
	}
	else if(gl_FragCoord.x < 475) {
        color = texture(screenTexture, TexCoords);
	}
    else {
// kernel
		const float offset = 1.0f / 300.0f;  

		vec2 offsets[9] = vec2[](
			vec2(-offset, offset),  // top-left
			vec2(0.0f,    offset),  // top-center
			vec2(offset,  offset),  // top-right
			vec2(-offset, 0.0f),    // center-left
			vec2(0.0f,    0.0f),    // center-center
			vec2(offset,  0.0f),    // center-right
			vec2(-offset, -offset), // bottom-left
			vec2(0.0f,    -offset), // bottom-center
			vec2(offset,  -offset)  // bottom-right    
		);

		// sharp
	//    float kernel[9] = float[](
	//        -1, -1, -1,
	//        -1,  9, -1,
	//        -1, -1, -1
	//    );
		
		// edge
	//    float kernel[9] = float[](
	//        1,  1,  1,
	//        1, -8,  1,
	//        1,  1,  1
	//    );
		
		// blur
		float kernel[9] = float[](
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16  
		);

		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++) {
			sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
		}
		
		vec3 col = vec3(0.0);
		
		for(int i = 0; i < 9; i++) {
			col += sampleTex[i] * kernel[i];
		}
	   
		color = vec4(col, 1.0);
	}
}