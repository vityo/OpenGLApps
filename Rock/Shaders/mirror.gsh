#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 mirrorTransforms[6];

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} gs_in[];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} gs_out;

//out int gl_Layer;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face;
		
        for(int i = 0; i < 3; ++i) {
			gs_out.Normal = gs_in[i].Normal;
			gs_out.TexCoords = gs_in[i].TexCoords;
			gs_out.FragPos = gl_in[i].gl_Position.xyz;
            gl_Position = mirrorTransforms[face] * gl_in[i].gl_Position;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 