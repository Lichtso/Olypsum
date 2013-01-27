in vec3 position;
uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
}

#separator

#define SAMPLES (SSAO_QUALITY*8)
#include random.glsl
const float sampleRadius = 12.0*SSAO_SCALE;
const float distanceScale = 50.0;

uniform sampler2DRect sampler0;
uniform vec2 pSphere[32];
out float colorOut;

void main() {
    int seed = genSeed();
	float depth = texture(sampler0, gl_FragCoord.xy*SSAO_SCALE).x;
	colorOut = 0.0;
	for(int i = 0; i < SAMPLES; i ++) {
        vec2 ray = reflect(pSphere[i], normalize(vec2rand(seed, vec2(1.0))))*sampleRadius;
		float sampleDepth = texture(sampler0, gl_FragCoord.xy*SSAO_SCALE+ray.xy).x;
		float occlusion = distanceScale * max(depth-sampleDepth, 0.0);
        colorOut += 1.0 / (1.0 + occlusion * occlusion * 0.5);
	}
	colorOut *= 1.0/float(SAMPLES);
}