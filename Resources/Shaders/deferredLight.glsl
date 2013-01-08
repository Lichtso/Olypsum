attribute vec3 position;

uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
}

#separator

#if SHADOWS_ACTIVE == 3
#extension GL_EXT_gpu_shader4 : require
#endif
#extension GL_ARB_texture_rectangle : enable

uniform vec3 camPos;
uniform float lInvRange;
uniform vec3 lColor, lDirection;
#if LIGHT_TYPE > 1
uniform float lCutoff;
uniform vec3 lPosition;
#endif

uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
uniform sampler2DRect sampler2;

#if SHADOWS_ACTIVE == 1
uniform sampler2DShadow sampler3;
#elif SHADOWS_ACTIVE == 2
uniform sampler2DShadow sampler3;
uniform sampler2DShadow sampler4;
#elif SHADOWS_ACTIVE == 3
uniform samplerCubeShadow sampler3;
uniform vec2 shadowDepthTransform;
#endif

#if SHADOWS_ACTIVE > 0
uniform mat4 lShadowMat;
#endif

#if LIGHT_TYPE == 1
#define depthBias 1.001
#define blurBias 0.001
#elif LIGHT_TYPE == 2
#define depthBias 0.999
#define blurBias 0.003
#elif LIGHT_TYPE == 3
#define depthBias 0.99*0.5
#define blurBias 0.001
#endif

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

#if SHADOW_QUALITY == 1
float shadowLookup2D(sampler2DShadow sampler, vec3 coord) {
    return shadow2D(sampler, coord).x;
}
#elif SHADOW_QUALITY > 1
float shadowLookup2D(sampler2DShadow sampler, vec3 coord) {
    float intensity = 0.0;
	const float blurSize = float(SHADOW_QUALITY)-1.0, blurSum = (blurSize*2.0+1.0)*(blurSize*2.0+1.0);
	for(float y = -blurSize; y <= blurSize; y += 1.0)
		for(float x = -blurSize; x <= blurSize; x += 1.0)
			intensity += shadow2D(sampler, vec3(coord.xy+vec2(x, y)*blurBias, coord.z)).x;
    return intensity / blurSum;
}
#endif

void main() {
    vec3 pos = texture2DRect(sampler0, gl_FragCoord.xy).xyz,
         normal = texture2DRect(sampler1, gl_FragCoord.xy).xyz,
         material = texture2DRect(sampler2, gl_FragCoord.xy).rgb;
    
    #if LIGHT_TYPE == 1
    vec3 lightDir = lDirection;
    float intensity = 1.0;
    #else
    vec3 lightDir = lPosition-pos;
    float lightDirLen = length(lightDir), intensity;
    lightDir /= lightDirLen;
    intensity = clamp(lightDirLen * lInvRange, step(lCutoff, dot(lDirection, lightDir)), 1.0);
    #endif

    #if SHADOW_QUALITY > 0 //Shadows enabled
    vec4 shadowCoord = vec4(pos, 1.0) * lShadowMat;
    #if LIGHT_TYPE < 3 //Directional or spot light
    shadowCoord.z = shadowCoord.z*depthBias*0.5+0.5*shadowCoord.w;
    intensity = (1.0-intensity)*shadowLookup2D(sampler3, shadowCoord.xyz/shadowCoord.w);
    #else //Positional light
    #if SHADOWS_ACTIVE < 3 //Parabolid
    #if SHADOWS_ACTIVE == 2
    if(shadowCoord.z < 0.0) {
    #endif
        shadowCoord.xy /= (lightDirLen - shadowCoord.z) * 2.0;
        shadowCoord.z = intensity * depthBias;
        shadowCoord.xyz += vec3(0.5);
        intensity = (1.0-intensity)*shadowLookup2D(sampler3, shadowCoord.xyz);
    #if SHADOWS_ACTIVE == 2
    }else{
        shadowCoord.x *= -1.0;
        shadowCoord.xy /= (lightDirLen + shadowCoord.z) * 2.0;
        shadowCoord.z = intensity * depthBias;
        shadowCoord.xyz += vec3(0.5);
        intensity = (1.0-intensity)*shadowLookup2D(sampler4, shadowCoord.xyz);
    }
    #endif
    #else //Cubemap
    shadowCoord.w = max(max(abs(shadowCoord.x), abs(shadowCoord.y)), abs(shadowCoord.z));
    shadowCoord.w = (shadowDepthTransform.x + shadowDepthTransform.y/shadowCoord.w);
    intensity = (1.0-intensity)*shadowCube(sampler5, shadowCoord).x;
    #endif //Cubemap
    #endif //Positional light
    #else //Shadows disabled
    intensity = (1.0-intensity);
    #endif //Shadows disabled
    
    gl_FragData[0].rgb = lColor*intensity*max(dot(lightDir, normal), 0.0);
    gl_FragData[1].rgb = lColor*intensity*pow(max(dot(reflect(lightDir, normal), normalize(pos-camPos)), 0.0), material.r*19.0+1.0)*material.g;
}