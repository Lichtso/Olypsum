attribute vec3 vertex;
attribute vec3 texCoord;
uniform mat4 modelViewMat;
varying vec2 vTexCoord;
varying float vAlpha;

void main() {
	gl_Position = vec4(vertex, 1.0)*modelViewMat;
	vTexCoord = texCoord.st;
	vAlpha = 1.0-texCoord.z;
}