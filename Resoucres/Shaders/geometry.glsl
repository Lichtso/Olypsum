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

#if BUMP_MAPPING
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
#if BUMP_MAPPING
varying vec3 vTangent;
varying vec3 vBitangent;

uniform vec3 camPos;
uniform sampler2D sampler[3];
#else
uniform sampler2D sampler[2];
#endif
uniform float discardDensity;

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	gl_FragData[0] = texture2D(sampler[0], vTexCoord);
    if(gl_FragData[0].a < 0.1 || random(gl_FragCoord.xy) > discardDensity) discard;
    
    #if BUMP_MAPPING
    vec4 bumpMap = texture2D(sampler[2], vTexCoord).rgba;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    gl_FragData[1].xyz = mat3(vTangent, vBitangent, vNormal)*bumpMap.xyz;
    #else
    gl_FragData[1].xyz = vNormal;
    #endif
    gl_FragData[1].a = 1.0;
    gl_FragData[2].rgb = texture2D(sampler[1], vTexCoord).rgb;
    gl_FragData[2].a = 1.0;
    gl_FragData[3].rgb = vPosition;
    gl_FragData[3].a = 1.0;
}