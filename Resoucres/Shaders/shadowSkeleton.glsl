attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute vec3 weights;
attribute vec3 joints;
uniform mat4 jointMats[64];
uniform mat4 viewMat;
uniform float paraboloidRange;
varying vec2 vTexCoord;
varying vec4 vPosition;

void main() {
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
    gl_Position = vec4(position, 1.0) * mat * viewMat;
	vTexCoord = texCoord;
    if(paraboloidRange == 0.0) {
        vPosition = vec4(0.5, 0.5, gl_Position.z*0.5, gl_Position.w);
    }else{
        gl_Position.xyz /= gl_Position.w;
        float len = length(gl_Position.xyz);
        gl_Position.z *= -1.0;
        vPosition.x = gl_Position.z+0.05;
        gl_Position.xy /= gl_Position.z + len;
        gl_Position.z = len / paraboloidRange;
        vPosition.yzw = vec3(1.0, gl_Position.z-0.5, 1.0);
        gl_Position.w = 1.0;
    }
}

#|#|#

uniform sampler2D sampler0;
uniform float discardDensity;
varying vec2 vTexCoord;
varying vec4 vPosition;

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    gl_FragColor = texture2D(sampler0, vTexCoord);
    if(gl_FragColor.a < 0.1 || vPosition.x < 0.0 || random(gl_FragCoord.xy) > discardDensity) discard;
    
    float depth = vPosition.z/vPosition.w+0.5;
    gl_FragColor.r = floor(depth*255.0)/255.0;
    gl_FragColor.g = floor((depth-gl_FragColor.r)*65535.0)/255.0;
    gl_FragColor.b = floor((depth-gl_FragColor.g)*16777215.0)/255.0;
    gl_FragColor.a = floor((depth-gl_FragColor.a)*4294967296.0)/255.0;
}