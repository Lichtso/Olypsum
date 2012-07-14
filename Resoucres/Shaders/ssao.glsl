attribute vec3 position;
uniform mat4 modelViewMat;
varying vec2 vTexCoord;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
    vTexCoord = position.xy*0.5+vec2(0.5);
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect sampler0;
uniform sampler2D sampler1;
uniform vec2 pSphere[32];
varying vec2 vTexCoord;

const float sampleRadius = 20.0;
const float distanceScale = 100.0;

#if SSAO_QUALITY < 4
#define SAMPLES (SSAO_QUALITY*8)
#else
#define SAMPLES 32
#endif

void main() {
	float depth = texture2DRect(sampler0, gl_FragCoord.xy*2.0).x;
	vec2 randNormal = texture2D(sampler1, vTexCoord * 10.0).xy*0.5+vec2(0.5);
    
	gl_FragData[0].x = 0.0;
	for(int i = 0; i < SAMPLES; i ++) {
		vec2 ray = reflect(pSphere[i], randNormal) * sampleRadius;
		float sampleDepth = texture2DRect(sampler0, gl_FragCoord.xy*2.0+ray.xy).x;
		float occlusion = distanceScale * max(depth-sampleDepth, 0.0);
        gl_FragData[0].x += 1.0 / (1.0 + occlusion * occlusion * 0.5);
	}
	gl_FragData[0].x *= 1.0/float(SAMPLES);
    gl_FragData[0].a = 1.0;
}