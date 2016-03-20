#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    //vec3 TangentLightPos;
    //vec3 TangentViewPos;
    //vec3 TangentFragPos;
	vec3 Normal;
	vec3 Tangent;
	//mat3 TBN;
	//mat3 TBNInversed;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform samplerCube texture_mirror;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	vec3 texture_diffuse_color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
	vec3 ambient = vec3(0.15f) * texture_diffuse_color;

	vec3 T = normalize(vec3(vec4(fs_in.Tangent, 0.0f)));
	vec3 N = normalize(vec3(vec4(fs_in.Normal, 0.0f)));
	T = normalize(T - dot(T, N) * N); // make orthogonal
	vec3 B = cross(T, N);
	mat3 TBN = mat3(T, B, N);
	
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
	normal = texture(texture_normal1, fs_in.TexCoords).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	normal = normalize(TBN * normal);
	
	//lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	//viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	lightDir = normalize(lightPos - fs_in.FragPos);
	viewDir = normalize(viewPos - fs_in.FragPos);
	
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = vec3(1.0f) * diff * texture_diffuse_color;
    
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
	
    vec3 specular = vec3(0.5f) * spec * texture(texture_specular1, fs_in.TexCoords).rgb; 
	
	//FragColor = vec4(texture(texture_mirror, fs_in.Normal).rgb, 1.0f);
	//FragColor = vec4(texture(texture_normal1, fs_in.TexCoords).rgb, 1.0f);
	
	//vec3 normalViewSpace = fs_in.TBN * normal;
	//vec3 normalViewSpace = fs_in.TBNInversed * normal;
	//FragColor = vec4(texture(texture_mirror, normalViewSpace).rgb, 1.0f);
	
	//FragColor = vec4(texture_diffuse_color, 1.0f);
	//FragColor = vec4(texture(texture_normal1, fs_in.TexCoords).rgb, 1.0f);
	//FragColor = vec4(ambient + diffuse + specular, 1.0f);
	//FragColor = vec4(texture(texture_mirror, -normal).rgb, 1.0f);
	FragColor = vec4(texture(texture_mirror, normal).rgb, 1.0f);
}