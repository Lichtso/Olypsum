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
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

void main() {
    #if SKELETAL_ANIMATION
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
    vec4 pos = vec4(position, 1.0) * mat;
    gl_Position = pos * viewMat;
    vPosition = pos.xyz/pos.w;
    vNormal = normalize((vec4(normal, 0.0) * mat).xyz);
    #else
    gl_Position = vec4(position, 1.0)*modelViewMat;
	vPosition = (vec4(position, 1.0)*modelMat).xyz;
    vNormal = normal*normalMat;
    #endif
	vTexCoord = texCoord;
}

#separator

uniform sampler2D sampler0; //ColorTexture
uniform sampler2D sampler1; //MaterialTexture
uniform float discardDensity;
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	gl_FragData[0] = texture2D(sampler0, vTexCoord);
    if(gl_FragData[0].a < 0.1 || random(gl_FragCoord.xy) > discardDensity) discard;
    
    gl_FragData[1].rgb = vPosition;
    gl_FragData[1].a = 1.0;
    gl_FragData[2].xyz = vNormal;
    gl_FragData[2].a = 1.0;
    gl_FragData[3] = texture2D(sampler1, vTexCoord);
    gl_FragData[3].a = 1.0;
}