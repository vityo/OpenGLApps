#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, float diff) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;
	
//	if (projCoords.x < 0.0f || projCoords.x > 1.0f
//		|| projCoords.y < 0.0f || projCoords.y > 1.0f) {
//		return 0.0f;
//	}
	if(projCoords.z > 1.0)
        return 0.0;
		
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
	float bias = 0.001f; //max(0.01f * (1.0f - diff), 0.005f); 
//    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

    return shadow;
}  

void main() {
	vec3 texture_color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	vec3 ambient = 0.3f * texture_color;
    vec3 lightColor = vec3(1.0f);
  	
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * lightColor;
    
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor; 
	
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace, diff);
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * texture_color;    
    
    FragColor = vec4(lighting, 1.0f);
}