attribute vec3 position;
attribute vec3 texCoord;
uniform mat4 modelViewMat;
varying vec2 vTexCoord;
varying float vAlpha;

void main() {
	gl_Position = vec4(position, 1.0)*modelViewMat;
	vTexCoord = texCoord.st;
	vAlpha = 1.0-texCoord.z;
}

#|#|#

uniform sampler2D sampler0;
uniform float light;
varying vec2 vTexCoord;
varying float vAlpha;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	gl_FragColor.rgb *= light;
	gl_FragColor.a *= vAlpha;
}