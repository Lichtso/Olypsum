attribute vec3 vertex;
attribute vec2 texCoord;
attribute vec3 normal;
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
uniform mat4 shadowMat;
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec4 sTexCoord;
varying vec3 vNormal;

void main() {
	gl_Position = vec4(vertex, 1.0)*modelViewMat;
	vPosition = (vec4(vertex, 1.0)*modelMat).xyz;
	vNormal = normal*normalMat;
	vTexCoord = texCoord;
	sTexCoord = vec4(vertex, 1.0)*shadowMat;
}