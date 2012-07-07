attribute vec3 position;

uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform mat4 camMat;
uniform sampler2DRect sampler[2];
uniform vec3 pSphere[16];
const float strength = 1.5;
const float falloff = 0.001;
const float rad = 30.0;

#define SAMPLES 10
const float invSamples = 1.0/float(SAMPLES);

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec3 occluderNormal, normal = texture2DRect(sampler[0], gl_FragCoord.xy).xyz;
    float depthDifference, depth = texture2DRect(sampler[1], gl_FragCoord.xy).x;
    vec3 ray, fres = vec3(random(gl_FragCoord.xy), random(gl_FragCoord.yx), 0.0), pos = vec3(gl_FragCoord.xy, depth);
    fres.z = random(vec2(fres.x, fres.y));
    fres = fres*2.0-vec3(1.0);
    
    //gl_FragData[0].r = sign(dot(camMat[2].xyz, normal));
    float bl = 0.0;
    for(int i = 0; i < SAMPLES; i ++) {
        ray = pos+rad*reflect(pSphere[i], fres);
        
        occluderNormal = texture2DRect(sampler[0], ray.xy).xyz;
        depthDifference = depth - texture2DRect(sampler[1], ray.xy).x;
        
        bl += step(falloff, depthDifference)*max(0.0, 1.0-dot(occluderNormal, normal));
        
        //float zd = 100.0*max(depthDifference, 0.0);
        //bl += 1.0/(1.0+zd*zd);
        //bl += step(falloff, depthDifference)*(1.0-dot(occluderNormal, normal))*strength;//*(1.0-smoothstep(0.0, strength, depthDifference));
    }
    gl_FragData[0].r = 1.0-invSamples*bl;
    gl_FragData[0].a = 1.0;
}