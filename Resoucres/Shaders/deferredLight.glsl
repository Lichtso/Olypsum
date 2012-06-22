attribute vec2 position;
varying vec2 vTexCoord;

void main() {
	gl_Position = vec4(position.x*2.0-1.0, position.y*2.0-1.0, 0.0, 1.0);
	vTexCoord = position;
}

#separator

uniform vec3 camPos;
uniform sampler2D sampler0; //DiffuseBuffer
uniform sampler2D sampler1; //SpecularBuffer
uniform sampler2D sampler2; //PositionBuffer
uniform sampler2D sampler3; //NormalBuffer
uniform sampler2D sampler4; //MaterialBuffer

uniform float lRange;
uniform vec3 lColor, lDirection;
#if LIGHT_TYPE > 1
uniform float lCutoff;
uniform vec3 lPosition;
#endif

#if SHADOWS_ACTIVE > 0
uniform mat4 lShadowMat;
uniform sampler2D sampler5;
#if SHADOWS_ACTIVE == 2
uniform sampler2D sampler6;
#endif
#if LIGHT_TYPE == 1
const float depthRefFactor = 0.99;
#elif LIGHT_TYPE == 2
const float depthRefFactor = 1.002;
#elif LIGHT_TYPE == 3
const float depthRefFactor = 1.02;
#endif
#endif

varying vec2 vTexCoord;

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec3 diffuseLight = texture2D(sampler0, vTexCoord).rgb,
         specularLight = texture2D(sampler1, vTexCoord).rgb,
         pos = texture2D(sampler2, vTexCoord).xyz,
         normal = texture2D(sampler3, vTexCoord).xyz,
         material = texture2D(sampler3, vTexCoord).rgb;
    
    #if LIGHT_TYPE == 1
    vec3 lightDir = lDirection;
    float intensity = 0.0;
    #else
    vec3 lightDir = lPosition-pos;
    float intensity = length(lightDir);
    lightDir /= intensity;
    intensity = 1.0-clamp(intensity / lRange, step(lCutoff, acos(-dot(lDirection, lightDir))), 1.0);
    #endif
    
    #if SHADOWS_ACTIVE
    /*if(intensity < 1.0) {
        float depthRef;
        vec4 shadowCoord = vec4(pos, 1.0)*lShadowMat;
        #if LIGHT_TYPE < 3
        shadowCoord.xyz /= shadowCoord.w;
        depthRef = texture2D(sampler5, shadowCoord.st, 0.0).r;
        #else
        float len = length(shadowCoord);
        #if SHADOWS_ACTIVE == 2
        if(shadowCoord.z < 0.0) {
        #endif
            shadowCoord.xy /= len - shadowCoord.z;
            shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
            depthRef = texture2D(sampler5, shadowCoord.st, 0.0).r;
        #if SHADOWS_ACTIVE == 2
        }else{
            shadowCoord.xy /= len + shadowCoord.z;
            shadowCoord.x *= -1.0;
            shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
            depthRef = texture2D(sampler6, shadowCoord.st, 0.0).r;
        }
        #endif
        shadowCoord.z = len;
        #endif
        intensity = max(intensity, step(depthRef*depthRefFactor, shadowCoord.z));
    }*/
    #endif
    
    //diffuseLight += lColor*intensity*max(dot(lightDir, normal), 0.0);
    //specularLight += lColor*intensity*pow(max(dot(reflect(lightDir, normal), normalize(camPos-pos)), 0.0), material.r)*material.g;
    
    gl_FragData[0].rgb = min(diffuseLight, 1.0);
    gl_FragData[0].a = 1.0;
    gl_FragData[1].rgb = min(specularLight, 1.0);
    gl_FragData[1].a = 1.0;
}