attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

#define lightsCount 3
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
	vec4 pos = vec4(position, 1.0)*modelMat;
	vNormal = normal*normalMat;
	vTexCoord = texCoord;
    for(int i = 0; i < lightsCount; i ++) {
        if(lightSources[i].type == 0.0) continue;
        if(lightSources[i].type == 1.0)
            lightDirs[i] = lightSources[i].direction;
        else
            lightDirs[i] = lightSources[i].position-pos.xyz;
        shadowCoords[i] = pos*lightSources[i].shadowMat;
    }
}

#|#|#

#define lightsCount 3
struct lightSource {
    float type, range, cutoff, shadowFactor;
	vec3 color, position, direction;
    mat4 shadowMat;
};

uniform lightSource lightSources[lightsCount];
uniform sampler2D sampler0;
uniform sampler2D shadowMaps[lightsCount*2];
uniform float discardDensity;
varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 lightDirs[lightsCount];
varying vec4 shadowCoords[lightsCount];

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
    if(gl_FragColor.a < 0.1 || random(gl_FragCoord.xy) > discardDensity) discard;
    vec3 shadowCoord, lightDir, light = vec3(0.2);
    vec2 shadowOffsetNoise;
    float lightIntensity, len;
    vec4 depthRef;
    const vec4 bitSh = vec4(1.0, 1.0/256.0, 1.0/65536.0, 1.0/16777216.0);
    
    for(int i = 0; i < lightsCount; i ++) {
        if(lightSources[i].type == 0.0) continue;
        lightDir = lightDirs[i];
        lightIntensity = length(lightDir);
        lightDir /= lightIntensity;
        lightIntensity = lightIntensity / lightSources[i].range;
        
        if(lightSources[i].shadowFactor > 0.0) {
            shadowCoord = shadowCoords[i].xyz / shadowCoords[i].w;
            if(lightSources[i].type < 3.0) {
                shadowOffsetNoise = vec2(random(shadowCoord.xy), random(shadowCoord.yx))*lightSources[i].shadowFactor;
                shadowCoord.z = shadowCoord.z*0.5+0.5;
                depthRef = texture2D(shadowMaps[i*2], shadowCoord.st+shadowOffsetNoise, 0.0);
                depthRef.r = dot(depthRef, bitSh)*(1.001+(lightSources[i].shadowFactor-0.01)*0.2);
            }else{
                shadowOffsetNoise = vec2(random(shadowCoord.xz), random(shadowCoord.zy))*lightSources[i].shadowFactor;
                len = length(shadowCoord);
                if(shadowCoord.z < 0.0) {
                    shadowCoord.xy /= len - shadowCoord.z;
                    shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
                    depthRef = texture2D(shadowMaps[i*2], shadowCoord.st+shadowOffsetNoise);
                }else{
                    shadowCoord.xy /= len + shadowCoord.z;
                    shadowCoord.x *= -1.0;
                    shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
                    depthRef = texture2D(shadowMaps[i*2+1], shadowCoord.st+shadowOffsetNoise);
                }
                shadowCoord.z = lightIntensity;
                depthRef.r = dot(depthRef, bitSh)*1.025;
            }
            lightIntensity = (1.0 - lightIntensity) * step(shadowCoord.z, depthRef.r);
        }else{
            lightIntensity = 1.0 - lightIntensity;
            if(lightSources[i].type == 2.0 && acos(-dot(lightSources[i].direction, lightDir)) > lightSources[i].cutoff)
                continue;
        }
        light += lightSources[i].color*lightIntensity*max(dot(vNormal, lightDir), 0.0);
    }
	
    gl_FragColor.rgb *= min(light, vec3(1.0));
}