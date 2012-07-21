#extension GL_EXT_gpu_shader4 : require

attribute vec4 position;
attribute vec3 velocity;
uniform float animationFactor;
uniform float lifeMin;
uniform float lifeRange;
uniform vec3 posMin;
uniform vec3 posRange;
uniform vec3 dirMin;
uniform vec3 dirRange;
uniform vec3 systemPosition;
uniform vec3 velocityAdd;
varying vec4 vPosition;
varying vec3 vVelocity;

const float InverseMaxInt = 1.0 / 4294967295.0;

float frand(int seed, float max) {
    int i=(seed^12345391)*2654435769;
    i^=(i<<6)^(i>>26);
    i*=2654435769;
    i+=(i<<5)^(i>>12);
    return float(max * i) * InverseMaxInt;
}

vec3 vec3rand(int seed, vec3 max) {
    return vec3(frand(seed, max.x), frand(seed, max.y), frand(seed, max.z));
}

void main() {
    vPosition.w = position.w-animationFactor;
    if(vPosition.w <= 0.0) {
        int seed = gl_VertexID;
        vPosition.xyz = vec3rand(seed ++, posRange)+posMin;
        vPosition.w = frand(seed ++, lifeRange)+lifeMin;
        vVelocity = vec3rand(seed, dirRange)+dirMin;
    }else{
        vPosition.xyz = position.xyz+velocity*animationFactor;
        vVelocity = velocity+velocityAdd;
    }
}