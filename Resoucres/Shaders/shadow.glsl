attribute vec3 position;
attribute vec2 texCoord;
uniform mat4 modelViewMat;
uniform mat4 viewMat;
uniform float lightType;
varying vec2 vTexCoord;
varying vec2 vPosition;

void main() {
	gl_Position = vec4(position, 1.0)*modelViewMat;
	vTexCoord = texCoord;
    vPosition = gl_Position.zw;
}

#|#|#

uniform float lightType;
uniform sampler2D sampler0;
varying vec2 vTexCoord;
varying vec2 vPosition;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	if(gl_FragColor.a < 0.1) discard;
    float depth = vPosition.x/vPosition.y*0.5+0.5;
	gl_FragColor.r = floor(depth*256.0)/256.0;
	gl_FragColor.g = floor(depth*65536.0-gl_FragColor.r*65536.0)/256.0;
	gl_FragColor.b = floor(depth*16711680.0-gl_FragColor.g*16711680.0)/256.0;
	gl_FragColor.a = 1.0;
}