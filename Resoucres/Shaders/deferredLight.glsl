attribute vec2 position;

void main() {
	gl_Position = vec4(position.x*2.0-1.0, position.y*2.0-1.0, 0.0, 1.0);
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform vec3 camPos;
uniform float lRange;
uniform vec3 lColor, lDirection;
#if LIGHT_TYPE > 1
uniform float lCutoff;
uniform vec3 lPosition;
#endif

uniform sampler2DRect sampler[5];

#if SHADOWS_ACTIVE
uniform sampler2DShadow shadowSampler;
#endif

#if SHADOWS_ACTIVE > 0
uniform mat4 lShadowMat;
#endif

#if LIGHT_TYPE == 1
const float depthBias = 1.005;
#elif LIGHT_TYPE == 2
const float depthBias = 0.999;
#elif LIGHT_TYPE == 3
const float depthBias = 0.99;
#endif

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec3 pos = texture2DRect(sampler[0], gl_FragCoord.xy).xyz,
         normal = texture2DRect(sampler[1], gl_FragCoord.xy).xyz,
         material = texture2DRect(sampler[2], gl_FragCoord.xy).rgb,
         diffuseLight = texture2DRect(sampler[3], gl_FragCoord.xy).rgb,
         specularLight = texture2DRect(sampler[4], gl_FragCoord.xy).rgb;
    
    #if LIGHT_TYPE == 1
    vec3 lightDir = lDirection;
    float intensity = 1.0;
    #else
    vec3 lightDir = lPosition-pos;
    float intensity = length(lightDir), lightDirLen = intensity;
    lightDir /= intensity;
    intensity = 1.0-clamp(intensity / lRange, step(dot(lDirection, lightDir), lCutoff), 1.0);
    #endif
    
    #if SHADOWS_ACTIVE
    vec4 shadowCoord = vec4(pos, 1.0) * lShadowMat;
    #if LIGHT_TYPE < 3
    shadowCoord.z = shadowCoord.z*0.5*depthBias+0.5*shadowCoord.w;
    intensity *= shadow2DProj(shadowSampler, shadowCoord).x;
    #else
    shadowCoord.xy /= lightDirLen - shadowCoord.z;
    shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
    shadowCoord.z = (1.0-intensity) * depthBias;
    intensity *= shadow2D(shadowSampler, shadowCoord.xyz).x;
    #endif
    #endif
    
    diffuseLight += lColor*intensity*max(dot(lightDir, normal), 0.0);
    specularLight += lColor*intensity*pow(max(dot(reflect(lightDir, normal), normalize(pos-camPos)), 0.0), material.r*10.0+1.0)*material.g;
    
    gl_FragData[0].rgb = min(diffuseLight, 1.0);
    gl_FragData[0].a = 1.0;
    gl_FragData[1].rgb = min(specularLight, 1.0);
    gl_FragData[1].a = 1.0;
}