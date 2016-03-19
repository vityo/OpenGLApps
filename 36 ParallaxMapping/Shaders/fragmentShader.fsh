#version 330 core
in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

out vec4 color;
  
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_depth;

uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) { 
    const float height_scale = 0.1f;
	// simple:
    //float height =  texture(texture_depth, texCoords).r;    
    //vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    //return texCoords - p;    
	
    const float minLayers = 8;
	const float maxLayers = 32;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir))); // в зависимости от угла обозрения увеличиваем количество слоев
    float layerDepth = 1.0f / numLayers;
    float currentLayerDepth = 0.0f;
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
	
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(texture_depth, currentTexCoords).r;
  
	while(currentLayerDepth < currentDepthMapValue) {
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(texture_depth, currentTexCoords).r;
		currentLayerDepth += layerDepth;  
	}
	
	//return currentTexCoords; // without interpolation
	
	// parallax occlusion mapping
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(texture_depth, prevTexCoords).r - currentLayerDepth + layerDepth;
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;  
} 

void main() {
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
	vec2 texCoords = fs_in.TexCoords;
	
	if(gl_FragCoord.x > 300) {
		viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
		
		if(gl_FragCoord.x > 500) {
			// with parallax mapping
			texCoords = ParallaxMapping(texCoords,  viewDir);
			
			if(texCoords.x < 0.0f || texCoords.x > 1.0f || texCoords.y < 0.0f || texCoords.y > 1.0f ) {
				discard;
			}
		}
		
		// with normal mapping
		lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
		normal = texture(texture_normal, texCoords).rgb;
		normal = normalize(normal * 2.0f - 1.0f);
	}
	else {
	//only blinn-phong
		viewDir = normalize(viewPos - fs_in.FragPos);
		lightDir = normalize(lightPos - fs_in.FragPos);
		normal = normalize(Normal);
	}
	
	vec3 texture_diffuse_color = texture(texture_diffuse, texCoords).rgb;
	vec3 ambient = 0.15f * texture_diffuse_color;
	
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = 1.0f * diff * texture_diffuse_color;
    
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
    vec3 specular = vec3(0.5f) * spec; 
	
    color = vec4(ambient + diffuse + specular, 1.0f);
}