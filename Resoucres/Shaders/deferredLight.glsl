attribute vec3 position;

uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
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

uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
uniform sampler2DRect sampler2;
uniform sampler2DRect sampler3;
uniform sampler2DRect sampler4;

#if SHADOWS_ACTIVE == 1
uniform sampler2DShadow sampler5;
#elif SHADOWS_ACTIVE == 2
uniform sampler2DShadow sampler5;
uniform sampler2DShadow sampler6;
#elif SHADOWS_ACTIVE == 3
uniform samplerCubeShadow sampler5;
#endif

#if SHADOWS_ACTIVE > 0 && SHADOWS_ACTIVE < 3
uniform mat4 lShadowMat;
#elif SHADOWS_ACTIVE == 3
uniform mat3 lShadowMat;
#endif

#if LIGHT_TYPE == 1
const float depthBias = 1.001;
#elif LIGHT_TYPE == 2
const float depthBias = 0.999;
#elif LIGHT_TYPE == 3
const float depthBias = 0.99*0.5;
#endif

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec3 pos = texture2DRect(sampler0, gl_FragCoord.xy).xyz,
         normal = texture2DRect(sampler1, gl_FragCoord.xy).xyz,
         material = texture2DRect(sampler2, gl_FragCoord.xy).rgb,
         diffuseLight = texture2DRect(sampler3, gl_FragCoord.xy).rgb,
         specularLight = texture2DRect(sampler4, gl_FragCoord.xy).rgb;
    
    #if LIGHT_TYPE == 1
    vec3 lightDir = lDirection;
    float intensity = 1.0;
    #else
    vec3 lightDir = lPosition-pos;
    float lightDirLen = length(lightDir), intensity;
    lightDir /= lightDirLen;
    intensity = 1.0-clamp(lightDirLen / lRange, step(dot(lDirection, lightDir), lCutoff), 1.0);
    #endif
    
    #if SHADOWS_ACTIVE
    #if LIGHT_TYPE < 3 || SHADOWS_ACTIVE < 3
    vec4 shadowCoord = vec4(pos, 1.0) * lShadowMat;
    #endif
    #if LIGHT_TYPE < 3
    shadowCoord.z = shadowCoord.z*depthBias*0.5+0.5*shadowCoord.w;
    intensity *= shadow2DProj(sampler5, shadowCoord).x;
    #else
    #if SHADOWS_ACTIVE == 1
    shadowCoord.xy /= lightDirLen - shadowCoord.z;
    shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
    shadowCoord.z = (1.0-intensity) * depthBias + 0.5;
    intensity *= shadow2D(sampler5, shadowCoord.xyz).x;
    #elif SHADOWS_ACTIVE == 2
    if(shadowCoord.z < 0.0) {
        shadowCoord.xy /= lightDirLen - shadowCoord.z;
        shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
        shadowCoord.z = (1.0-intensity) * depthBias + 0.5;
        intensity *= shadow2D(sampler5, shadowCoord.xyz).x;
    }else{
        shadowCoord.xy /= lightDirLen + shadowCoord.z;
        shadowCoord.x *= -1.0;
        shadowCoord.xy = shadowCoord.xy * 0.5 + vec2(0.5);
        shadowCoord.z = (1.0-intensity) * depthBias + 0.5;
        intensity *= shadow2D(sampler6, shadowCoord.xyz).x;
    }
    #elif SHADOWS_ACTIVE == 3
    vec3 shadowCoord = (pos-lPosition)*lShadowMat;
    intensity *= shadowCube(sampler5, shadowCoord).x;
    #endif
    #endif
    #endif
    
    diffuseLight += lColor*intensity*max(dot(lightDir, normal), 0.0);
    specularLight += lColor*intensity*pow(max(dot(reflect(lightDir, normal), normalize(pos-camPos)), 0.0), material.r*10.0+1.0)*material.g;
    
    gl_FragData[0].rgb = min(diffuseLight, 1.0);
    gl_FragData[0].a = 1.0;
    gl_FragData[1].rgb = min(specularLight, 1.0);
    gl_FragData[1].a = 1.0;
}