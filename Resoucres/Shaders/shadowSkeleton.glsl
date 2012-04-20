attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 weights;
attribute vec3 joints;
uniform mat4 jointMats[64];
uniform mat4 viewMat;
varying vec2 vTexCoord;

void main() {
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
	gl_Position = vec4(position, 1.0) * mat * viewMat;
	vTexCoord = texCoord;
}

#|#|#

uniform sampler2D sampler0;
varying vec2 vTexCoord;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	if(gl_FragColor.a < 0.2) discard;
	
	gl_FragColor.r = floor(gl_FragCoord.z*256.0)/256.0;
	gl_FragColor.g = floor(gl_FragCoord.z*65536.0-gl_FragColor.r*65536.0)/256.0;
	gl_FragColor.b = floor(gl_FragCoord.z*16711680.0-gl_FragColor.g*16711680.0)/256.0;
	gl_FragColor.a = 1.0;
}