attribute vec2 position;
uniform mat4 modelViewMat;
varying vec2 vTexCoord;

void main() {
	gl_Position = vec4(position.x*2.0-1.0, position.y*2.0-1.0, 0.0, 1.0);
	vTexCoord = position;
}

#|#|#

uniform sampler2D sampler0;
uniform float blurWidth;
varying vec2 vTexCoord;

void main() {
	gl_FragColor = vec4(0.0);
	float blurSize = 2.0;
	for(float y = -blurSize; y <= blurSize; y += 1.0)
		for(float x = -blurSize; x <= blurSize; x += 1.0)
			gl_FragColor += texture2D(sampler0, vTexCoord+vec2(x, y)*blurWidth);
	blurSize = blurSize*2.0+1.0;
	gl_FragColor /= blurSize*blurSize;
}