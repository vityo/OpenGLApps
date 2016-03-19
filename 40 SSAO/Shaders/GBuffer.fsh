#version 330 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

const float NEAR = 0.1f;
const float FAR = 50.0f;

float LinearizeDepth(float depth) {
    float z = depth * 2.0f - 1.0f;
    return (2.0f * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));	
}

void main() {    
    gPositionDepth.xyz = FragPos;
	gPositionDepth.a = 50.0f - LinearizeDepth(gl_FragCoord.z);
	
    gNormal = normalize(Normal);
    gAlbedo = vec3(1.0f, 0.95f, 1.0f);
} 