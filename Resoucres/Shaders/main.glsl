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
                shadowCoords[i] = vec4(pos, 1.0)*lightSources[i].shadowMat;
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
    vec3 shadowCoord, lightDir, light = vec3(0.2);
    float lightDist, len;
    vec4 depthRef;
    const vec4 bitSh = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
    
    for(int i = 0; i < lightsCount; i ++) {
        if(lightSources[i].type == 0.0) continue;
        lightDir = lightDirs[i];
        lightDist = length(lightDir);
        lightDir /= lightDist;
        lightDist /= lightSources[i].range; //Compiler bug: this statement has to be here
        if(lightSources[i].type == 3.0 && lightSources[i].shadowFactor > 0.0) {
            shadowCoord = shadowCoords[i].xyz / shadowCoords[i].w;
            len = length(shadowCoord);
            if(shadowCoord.z < 0.0) {
                shadowCoord.xy /= len * (1.0 - shadowCoord.z);
                shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
                depthRef = texture2D(shadowMaps[i*2], shadowCoord.st);
            }else{
                shadowCoord.xy /= len * (1.0 + shadowCoord.z);
                shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
                depthRef = texture2D(shadowMaps[i*2+1], shadowCoord.st);
            }
            lightDist = shadowCoord.z = len/lightSources[i].range;
            depthRef.r = dot(depthRef, bitSh)*lightSources[i].shadowFactor;
            if(depthRef.r < shadowCoord.z) continue;
        }else if(lightSources[i].type < 3.0) {
            shadowCoord = shadowCoords[i].xyz/shadowCoords[i].w;
            shadowCoord.z = shadowCoord.z*0.5+0.5;
            
            if(lightSources[i].type == 1.0 && (shadowCoord.x <= 0.0 || shadowCoord.y <= 0.0 || shadowCoord.x >= 1.0 || shadowCoord.y >= 1.0))
                continue;
            else if(lightSources[i].type == 2.0 && acos(-dot(lightSources[i].direction, lightDir)) > lightSources[i].cutoff)
                continue;
            if(lightSources[i].shadowFactor > 0.0) {
                depthRef = texture2D(shadowMaps[i*2], shadowCoord.st);
                depthRef.r = dot(depthRef, bitSh)*lightSources[i].shadowFactor;
                if(depthRef.r < shadowCoord.z) continue;
            }
        }
        light += lightSources[i].color*max(dot(vNormal, lightDir), 0.0)*(1.0-lightDist);
    }
	
    gl_FragColor.rgb *= min(light, vec3(1.0));
}