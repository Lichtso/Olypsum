attribute vec3 vertex;
attribute vec2 texCoord;
uniform mat4 modelViewMat;
varying vec2 vTexCoord;
varying float vDepth;

void main() {
	gl_Position = vec4(vertex, 1.0)*modelViewMat;
	vTexCoord = texCoord;
	vDepth = -gl_Position.z;
}