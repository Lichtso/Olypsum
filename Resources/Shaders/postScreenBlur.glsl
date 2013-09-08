in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

out vec3 colorOut;
uniform sampler2DRect sampler0;
uniform float processingValue;

#include random.glsl
const int blurWidth = 10;
const float blurInverse = 1.0/float(blurWidth);

void main() {
    int seed = genSeed();
    colorOut = vec3(0.0);
	for(int i = 0; i < blurWidth; i ++)
		colorOut += texture(sampler0, gl_FragCoord.xy+vec2SeedRand(seed, vec2(processingValue))).rgb;
	colorOut *= blurInverse;
}