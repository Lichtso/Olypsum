in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

out float colorOut;
uniform sampler2DRect sampler0;
uniform vec2 pSphere[32];

#include random.glsl
const float sampleRadius = 12.0*SSAO_SCALE;
const float distanceScale = 50.0;
const int samples = int(8)*SSAO_QUALITY;

void main() {
	int seed = genSeed();
	float depth = texture(sampler0, gl_FragCoord.xy*SSAO_SCALE).x;
	colorOut = 0.0;
	for(int i = 0; i < samples; i ++) {
	    vec2 ray = reflect(pSphere[i], normalize(vec2SeedRand(seed, vec2(1.0))))*sampleRadius;
		float sampleDepth = texture(sampler0, gl_FragCoord.xy*SSAO_SCALE+ray.xy).x;
		float occlusion = distanceScale * max(depth-sampleDepth, 0.0);
	    colorOut += 1.0 / (1.0 + occlusion * occlusion * 0.5);
	}
	colorOut *= 1.0/float(samples);
}