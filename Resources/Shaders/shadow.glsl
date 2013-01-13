in vec3 position;
in vec2 texCoord;
#if SKELETAL_ANIMATION
in vec3 weights;
in vec3 joints;

uniform mat4 jointMats[64];
uniform mat4 viewMat;
#else
uniform mat4 modelViewMat;
#endif

#if PARABOLID == 1
uniform float lRange;
out float vClip;
#endif
out vec2 vTexCoord;

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
    
    #if PARABOLID == 1
    gl_Position.xyz /= gl_Position.w;
    gl_Position.w = 1.0;
    gl_Position.z *= -1.0;
    vClip = gl_Position.z+0.05;
    float len = length(gl_Position.xyz);
    gl_Position.xy /= len + gl_Position.z;
    gl_Position.z = len / lRange;
    #endif
    
    vTexCoord = texCoord;
}

#separator

#if TEXTURE_ANIMATION == 0
uniform sampler2D sampler0;
#else
uniform vec2 texCoordAnimF;
uniform vec2 texCoordAnimZ;
uniform sampler2DArray sampler0;
#endif
uniform float paraboloidRange;
uniform float discardDensity;

#if PARABOLID == 1
in float vClip;
#endif
in vec2 vTexCoord;

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    #if TEXTURE_ANIMATION == 0 //2D texture
    float alpha = texture(sampler0, vTexCoord).a;
    #else //3D texture
    float alpha = texture(sampler0, vec3(vTexCoord.xy, texCoordAnimZ.x+0.5)).a;
    #endif
    
    #if PARABOLID == 1
    if(alpha < 0.1 || vClip < 0.0 || random(gl_FragCoord.xy) > discardDensity) discard;
    #else
    if(alpha < 0.1 || random(gl_FragCoord.xy) > discardDensity) discard;
    #endif
}