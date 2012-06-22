attribute vec3 position;
attribute vec2 texCoord;
#if SKELETAL_ANIMATION
attribute vec3 weights;
attribute vec3 joints;


uniform mat4 jointMats[64];
uniform mat4 viewMat;
#else
uniform mat4 modelViewMat;
uniform mat4 modelMat;
#endif

varying vec2 vTexCoord;
varying vec2 vPosition;

void main() {
    #if SKELETAL_ANIMATION
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
    gl_Position = vec4(position, 1.0)*mat*viewMat;
    #else
    gl_Position = vec4(position, 1.0)*modelViewMat;
    #endif
    
    #if LIGHT_TYPE == 2
    gl_Position.z *= -1.0;
    vPosition.x = gl_Position.z+0.05;
    float len = length(gl_Position.xyz);
    gl_Position.xy /= len + gl_Position.z;
    gl_Position.z = vPosition.y = len;
    gl_Position.w = 1.0;
    #else
    vPosition = vec2(gl_Position.z, gl_Position.w);
    #endif
    
    vTexCoord = texCoord;
}

#separator

uniform sampler2D sampler0; //ColorTexture
uniform float paraboloidRange;
uniform float discardDensity;
varying vec2 vTexCoord;
varying vec2 vPosition;

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    gl_FragColor = texture2D(sampler0, vTexCoord);
    
    #if LIGHT_TYPE == 2
    if(gl_FragColor.a < 0.1 || vPosition.x < 0.0 || random(gl_FragCoord.xy) > discardDensity) discard;
    gl_FragColor.r = vPosition.y;
    #else
    if(gl_FragColor.a < 0.1 || random(gl_FragCoord.xy) > discardDensity) discard;
    gl_FragColor.r = vPosition.x/vPosition.y;
    #endif
    
    gl_FragColor.a = 1.0;
}