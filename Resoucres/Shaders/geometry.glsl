attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
#if SKELETAL_ANIMATION
attribute vec3 weights;
attribute vec3 joints;


uniform mat4 jointMats[64];
uniform mat4 viewMat;
#else
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
#endif

#if BUMP_MAPPING > 0
varying vec3 gPosition;
varying vec2 gTexCoord;
varying vec3 gNormal;
#else
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
#endif

void main() {
    #if SKELETAL_ANIMATION
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
    vec4 pos = vec4(position, 1.0) * mat;
    gl_Position = pos * viewMat;
    #if BUMP_MAPPING
    gPosition = pos.xyz/pos.w;
    gNormal = normalize((vec4(normal, 0.0) * mat).xyz);
    gTexCoord = texCoord;
    #else
    vPosition = pos.xyz/pos.w;
    vNormal = normalize((vec4(normal, 0.0) * mat).xyz);
    vTexCoord = texCoord;
    #endif
    #else
    gl_Position = vec4(position, 1.0)*modelViewMat;
    #if BUMP_MAPPING
    gPosition = (vec4(position, 1.0)*modelMat).xyz;
    gNormal = normal*normalMat;
    gTexCoord = texCoord;
    #else
	vPosition = (vec4(position, 1.0)*modelMat).xyz;
    vNormal = normal*normalMat;
    vTexCoord = texCoord;
    #endif
    #endif
}

#separator

varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
#if BUMP_MAPPING > 0
varying vec3 vTangent;
varying vec3 vBitangent;

uniform vec3 camPos;
uniform sampler2D sampler2;
#endif
uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform float discardDensity;

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

#if BUMP_MAPPING == 3
void LinearParallax(inout vec3 texCoord, vec3 viewVec, float steps) {
	for(float i = 0.0; i < steps; i ++) {
		if(texCoord.z >= texture2D(sampler2, texCoord.xy).a)
            return;
        texCoord += viewVec;
    }
}

void BinaryParallax(inout vec3 texCoord, vec3 viewVec) {
	for(int i = 0; i < 6; i ++) {
		if(texCoord.z < texture2D(sampler2, texCoord.xy).a)
            texCoord += viewVec;
		viewVec *= 0.5;
		texCoord -= viewVec;
	}
}
#endif

void main() {
    #if BUMP_MAPPING <= 1
	gl_FragData[0] = texture2D(sampler0, vTexCoord);
    if(gl_FragData[0].a < 0.1 || random(gl_FragCoord.xy) > discardDensity) discard;
    gl_FragData[2].rgb = texture2D(sampler1, vTexCoord).rgb;
    #endif
    
    vec3 normal = normalize(vNormal);
    #if BUMP_MAPPING == 0
    gl_FragData[1].xyz = normal;
    #elif BUMP_MAPPING == 1
    vec3 bumpMap = texture2D(sampler2, vTexCoord).xyz;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    gl_FragData[1].xyz = mat3(vTangent, vBitangent, normal)*bumpMap;
    #elif BUMP_MAPPING > 1
    if(random(gl_FragCoord.xy) > discardDensity) discard;
    vec3 viewVec = normalize(camPos-vPosition);
    #if BUMP_MAPPING == 2
    viewVec.xy = vec2(dot(viewVec, vTangent), dot(viewVec, vBitangent));
    vec2 texCoord = vTexCoord-viewVec.xy*texture2D(sampler2, vTexCoord).a*0.04;
    #elif BUMP_MAPPING == 3
    viewVec = vec3(dot(viewVec, vTangent), dot(viewVec, vBitangent), dot(viewVec, normal));
    float steps = floor((1.0 - viewVec.z) * 18.0) + 2.0;
    viewVec.xy *= -0.04/viewVec.z;
    viewVec.z = 1.0;
    viewVec /= steps;
    vec3 texCoord = vec3(vTexCoord, 0.0);
    LinearParallax(texCoord, viewVec, steps);
	BinaryParallax(texCoord, viewVec);
    #endif
    gl_FragData[0] = texture2D(sampler0, texCoord.xy);
    if(abs(texCoord.x-0.5) > 0.5 || abs(texCoord.y-0.5) > 0.5 || gl_FragData[0].a < 0.1 || random(gl_FragCoord.xy) > discardDensity) discard;
    gl_FragDepth = gl_FragCoord.z+length(texCoord.xy-vTexCoord)*0.2;
    vec3 bumpMap = texture2D(sampler2, texCoord.xy).xyz;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    gl_FragData[1].xyz = mat3(vTangent, vBitangent, normal)*bumpMap;
    gl_FragData[2].rgb = texture2D(sampler1, texCoord.xy).rgb;
    #endif
    
    gl_FragData[3].rgb = vPosition;
    gl_FragData[1].a = 1.0;
    gl_FragData[2].a = 1.0;
    gl_FragData[3].a = 1.0;
}