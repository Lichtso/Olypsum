in vec3 position;
uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
}

#separator

uniform sampler2DRect sampler0;
uniform vec2 pSphere[32];
out float colorOut;

const float sampleRadius = 12.0*SSAO_SCALE;
const float distanceScale = 50.0;

#define SAMPLES (SSAO_QUALITY*8)

const float InverseMaxInt = 1.0 / 4294967295.0;

float frand(inout int seed) {
    int i=(seed^12345391)*2654435769;
    i^=(i<<6)^(i>>26);
    i*=2654435769;
    i+=(i<<5)^(i>>12);
    seed ++;
    return float(i) * InverseMaxInt;
}

void main() {
    int seed = (int(gl_FragCoord.x)^12345391)*2654435769;
    seed^=(seed<<6)^(seed>>26);
    seed*=int(gl_FragCoord.y);
    seed+=(seed<<5)^(seed>>12);
    
	float depth = texture(sampler0, gl_FragCoord.xy*SSAO_SCALE).x;
	colorOut = 0.0;
	for(int i = 0; i < SAMPLES; i ++) {
        vec2 ray = reflect(pSphere[i], normalize(vec2(frand(seed), frand(seed))))*sampleRadius;
		float sampleDepth = texture(sampler0, gl_FragCoord.xy*SSAO_SCALE+ray.xy).x;
		float occlusion = distanceScale * max(depth-sampleDepth, 0.0);
        colorOut += 1.0 / (1.0 + occlusion * occlusion * 0.5);
	}
	colorOut *= 1.0/float(SAMPLES);
}