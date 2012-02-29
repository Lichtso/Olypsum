uniform sampler2D sampler0;
varying vec2 vTexCoord;
varying float vDepth;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	if(gl_FragColor.a < 0.2) discard;
	
	gl_FragColor.r = floor(vDepth*256.0)/256.0;
	gl_FragColor.g = floor(vDepth*65536.0-gl_FragColor.r*65536.0)/256.0;
	gl_FragColor.b = floor(vDepth*16711680.0-gl_FragColor.g*16711680.0)/256.0;
	gl_FragColor.a = 1.0;
}