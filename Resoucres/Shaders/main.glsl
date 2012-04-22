attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

#define lightsCount 4
struct lightSource {
    float type, range, cutoff, shadowFactor;
	vec3 color, position, direction;
    mat4 shadowMat;
};

uniform lightSource lightSources[lightsCount];
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 lightDirs[lightsCount];
varying vec4 shadowCoords[lightsCount];

void main() {
	gl_Position = vec4(position, 1.0)*modelViewMat;
	vec3 pos = (vec4(position, 1.0)*modelMat).xyz;
	vNormal = normal*normalMat;
	vTexCoord = texCoord;
    for(int i = 0; i < lightsCount; i ++) {
        if(lightSources[i].type == 0.0) continue;
        if(lightSources[i].type == 1.0) {
            lightDirs[i] = lightSources[i].direction;
        }else{
            lightDirs[i] = lightSources[i].position-pos;
            if(lightSources[i].type == 3.0) {
                
            }
        }
        if(lightSources[i].shadowFactor > 0.0 && lightSources[i].type <= 2.0)
            shadowCoords[i] = vec4(pos, 1.0)*lightSources[i].shadowMat;
    }
}

#|#|#

#define lightsCount 4
struct lightSource {
    float type, range, cutoff, shadowFactor;
	vec3 color, position, direction;
    mat4 shadowMat;
};

uniform lightSource lightSources[lightsCount];
uniform sampler2D sampler0;
uniform sampler2D shadowMaps[lightsCount*2];
varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 lightDirs[lightsCount];
varying vec4 shadowCoords[lightsCount];

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
    if(gl_FragColor.a < 0.1) discard;
    
    vec3 light = vec3(0.2);
    for(int i = 0; i < lightsCount; i ++) {
        if(lightSources[i].type == 0.0) continue;
        vec3 shadowCoord = shadowCoords[i].xyz/shadowCoords[i].w;
        shadowCoord.z = shadowCoord.z*0.5+0.5;
        
        float len = length(lightDirs[i]);
        if(lightSources[i].type == 3.0) {
            
        }else{
            if(lightSources[i].type == 1.0 && (shadowCoord.x <= 0.0 || shadowCoord.y <= 0.0 || shadowCoord.x >= 1.0 || shadowCoord.y >= 1.0))
                continue;
            else if(lightSources[i].type == 2.0 && acos(-dot(lightSources[i].direction, lightDirs[i])/len) > lightSources[i].cutoff)
                continue;
            if(lightSources[i].shadowFactor > 0.0) {
                vec3 depthRef = texture2D(shadowMaps[i], shadowCoord.st).rgb;
                depthRef.r = (depthRef.r+depthRef.g/256.0+depthRef.b/65536.0)*lightSources[i].shadowFactor;
                if(depthRef.r < shadowCoord.z) continue;
            }
        }
        light += lightSources[i].color*(1.0-len/lightSources[i].range)*max(dot(vNormal, lightDirs[i]/len), 0.0);
    }
	
    gl_FragColor.rgb *= min(light, vec3(1.0));
}