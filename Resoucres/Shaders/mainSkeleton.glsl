attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute vec3 weights;
attribute vec3 joints;
uniform mat4 jointMats[64];
uniform mat4 viewMat;
varying vec2 vTexCoord;
varying vec3 vNormal;

void main() {
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
	gl_Position = vec4(position, 1.0) * (mat);
	vTexCoord = texCoord;
    vNormal = normalize((vec4(normal, 0.0) * mat).xyz);
}

#|#|#

uniform sampler2D sampler0;
varying vec2 vTexCoord;
varying vec3 vNormal;

void main() {
    gl_FragColor.rgb = vNormal;
    gl_FragColor = texture2D(sampler0, vTexCoord);
}