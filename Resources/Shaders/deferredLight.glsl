in vec3 position;

uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
}

#separator

out vec3 diffuseOut;
out vec3 specularOut;

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
vec3 shadowReflector[6] = vec3[](
    vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0),
    vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0)
);
#endif
uniform mat4 lShadowMat;

#if LIGHT_TYPE == 1
#define depthOffset -0.002
#define blurBias 0.001
#elif LIGHT_TYPE == 2
#define depthOffset -0.002
#define blurBias 0.003
#elif LIGHT_TYPE == 3
#if SHADOWS_ACTIVE < 3
#define depthBias 0.99*0.5
#define blurBias 0.001
#else
#define depthBias 0.99
#define blurBias 0.002
#endif
#endif

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

#if SHADOW_QUALITY == 1
float shadowLookup2D(sampler2DShadow sampler, vec3 coord) {
    return texture(sampler, coord);
}
#elif SHADOW_QUALITY > 1
float shadowLookup2D(sampler2DShadow sampler, vec3 coord) {
    float shadowSum = 0.0;
	const float blurSize = float(SHADOW_QUALITY)-1.0, blurSum = (blurSize*2.0+1.0)*(blurSize*2.0+1.0);
	for(float y = -blurSize; y <= blurSize; y += 1.0)
		for(float x = -blurSize; x <= blurSize; x += 1.0)
			shadowSum += texture(sampler, vec3(coord.xy+vec2(x, y)*blurBias, coord.z));
    return shadowSum/blurSum;
}
#endif

void main() {
    vec3 pos = texture(sampler0, gl_FragCoord.xy).xyz,
         normal = texture(sampler1, gl_FragCoord.xy).xyz,
         material = texture(sampler2, gl_FragCoord.xy).rgb;
    
    #if SHADOW_QUALITY > 0 || LIGHT_TYPE == 1
    vec4 shadowCoord = vec4(pos, 1.0) * lShadowMat;
    #if LIGHT_TYPE < 3
    shadowCoord.z = (shadowCoord.w+shadowCoord.z)*0.5+depthOffset;
    #endif
    #endif
    
    #if LIGHT_TYPE == 1
    vec3 lightDir = lDirection;
    float intensity = 1.0-step(abs(shadowCoord.x-0.5), 0.5)*step(abs(shadowCoord.y-0.5), 0.5)*step(abs(shadowCoord.z-0.5), 0.5);
    #else
    vec3 lightDir = lPosition-pos;
    float lightDirLen = length(lightDir), intensity;
    lightDir /= lightDirLen;
    intensity = clamp(lightDirLen * lInvRange, step(lCutoff, dot(lDirection, lightDir)), 1.0);
    #endif
    
    //if(intensity >= 1.0) discard;
    
    #if SHADOW_QUALITY > 0 //Shadows enabled
    #if LIGHT_TYPE < 3 //Directional or spot light
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
    shadowCoord.w = max(max(abs(shadowCoord.x), abs(shadowCoord.y)), abs(shadowCoord.z))*depthBias;
    #if SHADOW_QUALITY == 1
    shadowCoord.w = (shadowDepthTransform.x + shadowDepthTransform.y/shadowCoord.w);
    intensity = (1.0-intensity)*texture(sampler3, shadowCoord);
    #elif SHADOW_QUALITY > 1 //Soft Shadows
    int sideIndex = 0;
    if(abs(shadowCoord.y) == shadowCoord.w)
        sideIndex = 2;
    else if(abs(shadowCoord.z) == shadowCoord.w)
        sideIndex = 4;
    shadowCoord.w = (shadowDepthTransform.x + shadowDepthTransform.y/shadowCoord.w);
    shadowCoord.xyz = normalize(shadowCoord.xyz);
    vec3 rightVec = shadowReflector[sideIndex+1]*blurBias;
    vec3 upVec = shadowReflector[sideIndex]*blurBias;
    float shadowSum = 0.0;
	const float blurSize = float(SHADOW_QUALITY)-1.0, blurSum = (blurSize*2.0+1.0)*(blurSize*2.0+1.0);
	for(float y = -blurSize; y <= blurSize; y += 1.0)
		for(float x = -blurSize; x <= blurSize; x += 1.0)
			shadowSum += texture(sampler3, vec4(shadowCoord.xyz+rightVec*x+upVec*y, shadowCoord.w));
    intensity = (1.0-intensity)*(shadowSum/blurSum);
    #endif //Soft Shadows
    #endif //Cubemap
    #endif //Positional light
    #else //Shadows disabled
    intensity = (1.0-intensity);
    #endif //Shadows disabled
    
    intensity *= intensity;
    diffuseOut = lColor*intensity*max(dot(lightDir, normal), 0.0);
    specularOut = lColor*intensity*pow(max(dot(reflect(lightDir, normal), normalize(pos-camPos)), 0.0), material.r*19.0+1.0)*material.g;
}