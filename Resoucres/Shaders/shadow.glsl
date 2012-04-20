attribute vec3 position;
attribute vec2 texCoord;
uniform mat4 modelViewMat;
varying vec2 vTexCoord;
varying float vDepth;

void main() {
	gl_Position = vec4(position, 1.0)*modelViewMat;
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