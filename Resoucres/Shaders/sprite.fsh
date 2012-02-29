uniform sampler2D sampler0;
uniform float light;
varying vec2 vTexCoord;
varying float vAlpha;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	gl_FragColor.rgb *= light;
	gl_FragColor.a *= vAlpha;
}