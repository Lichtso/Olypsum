attribute vec2 position;
uniform mat4 modelViewMat;

void main() {
	gl_Position = vec4(position.x*2.0-1.0, position.y*2.0-1.0, 0.0, 1.0);
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect sampler[1];
uniform float blurWidth;

void main() {
	gl_FragColor = vec4(0.0);
	const float blurSize = 2.0;
	for(float y = -blurSize; y <= blurSize; y += 1.0)
		for(float x = -blurSize; x <= blurSize; x += 1.0)
			gl_FragColor += texture2DRect(sampler[0], gl_FragCoord.xy+vec2(x, y)*blurWidth);
	gl_FragColor /= (blurSize*2.0+1.0)*(blurSize*2.0+1.0);
}