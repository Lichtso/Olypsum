attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute vec3 weights;
attribute vec3 joints;
uniform mat4 jointMats[64];
uniform mat4 viewMat;
uniform float paraboloidRange;
varying vec2 vTexCoord;
varying vec3 vPosition;

void main() {
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
    gl_Position = vec4(position, 1.0) * mat * viewMat;
	vTexCoord = texCoord;
    if(paraboloidRange == 0.0) {
        vPosition = vec3(gl_Position.z*0.5, gl_Position.w, 0.5);
    }else{
        gl_Position.xyz /= gl_Position.w;
        float len = length(gl_Position.xyz);
        gl_Position.z *= -1.0;
        vPosition.z = gl_Position.z;
        gl_Position.xy /= len * (gl_Position.z + 1.0);
        gl_Position.z = len / paraboloidRange;
        vPosition.xy = vec2(gl_Position.z-0.5, 1.0);
        gl_Position.w = 1.0;
    }
}

#|#|#

uniform sampler2D sampler0;
varying vec2 vTexCoord;
varying vec3 vPosition;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	if(gl_FragColor.a < 0.1 || vPosition.z < 0.0) discard;
    const vec4 bitSh = vec4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
    const vec4 bitMsk = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
    gl_FragColor = fract((vPosition.x/vPosition.y+0.5) * bitSh);
    gl_FragColor -= gl_FragColor.xxyz * bitMsk;
}