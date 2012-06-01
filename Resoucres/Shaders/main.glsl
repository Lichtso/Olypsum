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
            lightDirs[i] = lightSources[i].position-pos.xyz/pos.w;
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
    vec4 depthRef;
    vec3 shadowCoord, lightDir, light = vec3(0.2);
    vec2 shadowOffsetNoise;
    float lightIntensity, len, depthRefFactor, shadowMap;
    const vec4 bitSh = vec4(1.0, 1.0/256.0, 1.0/65536.0, 1.0/16777216.0);
    
    for(int i = 0; i < lightsCount; i ++) {
        if(lightSources[i].type == 0.0) continue;
        lightDir = lightDirs[i];
        lightIntensity = length(lightDir);
        lightDir /= lightIntensity;
        lightIntensity = clamp(lightIntensity / lightSources[i].range, step(lightSources[i].cutoff, acos(-dot(lightSources[i].direction, lightDir))), 1.0);
        
        if(lightIntensity < 1.0 && lightSources[i].shadowFactor > 0.0) {
            shadowCoord = shadowCoords[i].xyz / shadowCoords[i].w;
            shadowMap = float(i*2);
            if(lightSources[i].type < 3.0) {
                shadowOffsetNoise = vec2(random(shadowCoord.xy), random(shadowCoord.yx))*lightSources[i].shadowFactor;
                shadowCoord.z = shadowCoord.z*0.5+0.5;
                depthRefFactor = 1.001+(lightSources[i].type-1.0)*0.001;
            }else{
                shadowOffsetNoise = vec2(random(shadowCoord.xz), random(shadowCoord.zy))*lightSources[i].shadowFactor;
                len = length(shadowCoord);
                float backFactor = step(shadowCoord.z, 0.0)*2.0-1.0;
                shadowCoord.xy /= len - shadowCoord.z * backFactor;
                shadowCoord.x *= backFactor;
                shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
                shadowMap += 0.5-backFactor*0.5;
                shadowCoord.z = lightIntensity;
                depthRefFactor = 1.025;
            }
            depthRef = texture2D(shadowMaps[int(shadowMap)], shadowCoord.st+shadowOffsetNoise, 0.0);
            lightIntensity = max(lightIntensity, step(dot(depthRef, bitSh)*depthRefFactor, shadowCoord.z));
        }
        light += lightSources[i].color*(1.0-lightIntensity)*max(dot(vNormal, lightDir), 0.0);
    }
	
    gl_FragColor.rgb *= min(light, vec3(1.0));
}