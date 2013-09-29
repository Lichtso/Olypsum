in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

out float colorOut;
uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
uniform vec3 pSphere[32];
uniform mat4 camMat;

#include random.glsl
const float sampleRadius = 30.0*SSAO_SCALE;
const float distanceScale = 80.0;
const int samples = int(8)*SSAO_QUALITY;

void main() {
    int seed = genSeed();
    vec3 normal = texture(sampler0, gl_FragCoord.xy*SSAO_SCALE).xyz;
    normal = vec3(dot(normal, camMat[0].xyz), dot(normal, camMat[1].xyz), dot(normal, camMat[2].xyz));
    vec3 tangent = vec3SeedRand(seed, vec3(1.0));
    tangent = normalize(tangent - normal * dot(tangent, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 normalMat = mat3(tangent, bitangent, normal);
    
    float depth = texture(sampler1, gl_FragCoord.xy*SSAO_SCALE).x;
    colorOut = 0.0;
    for(int i = 0; i < samples; i ++) {
	    vec2 delta = (normalMat*pSphere[i]).xy*sampleRadius;
		float occlusion = max(depth-texture(sampler1, gl_FragCoord.xy*SSAO_SCALE+delta).x, 0.0)*distanceScale;
        colorOut += 1.0 / (occlusion+1.0);
	}
	colorOut *= 1.0/float(samples);
}