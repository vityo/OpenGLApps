#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    //vec3 TangentLightPos;
    //vec3 TangentViewPos;
    //vec3 TangentFragPos;
	vec3 Normal;
	vec3 Tangent;
	//mat3 TBN;
	//mat3 TBNInversed;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.TexCoords = texCoords;
	mat3 normalMatrix = mat3(transpose(inverse(model)));
    vs_out.Normal = normalMatrix * normal;
    vs_out.Tangent = normalMatrix * tangent;
	
	//vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	//vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
	//T = normalize(T - dot(T, N) * N); // make orthogonal
	//vec3 B = cross(T, N);
	//vs_out.TBN = mat3(T, B, N);
   // mat3 TBNInversed = transpose(TBN); // inverse
	//vs_out.TBNInversed = TBNInversed;
	
	//vs_out.TangentLightPos = TBNInversed * lightPos;
    //vs_out.TangentViewPos  = TBNInversed * viewPos;
    //vs_out.TangentFragPos  = TBNInversed * vs_out.FragPos;
	
}