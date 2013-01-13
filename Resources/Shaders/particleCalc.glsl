in vec4 position;
in vec4 velocity;
uniform float respawnParticles;
uniform float animationFactor;
uniform float lifeCenter;
uniform float lifeRange;
uniform float sizeCenter;
uniform float sizeRange;
uniform vec3 posCenter;
uniform vec3 posRange;
uniform vec3 dirCenter;
uniform vec3 dirRange;
uniform vec3 velocityAdd;
out vec4 vPosition;
out vec4 vVelocity;

const float InverseMaxInt = 1.0 / 4294967295.0;

float frand(inout int seed, float max) {
    int i=(seed^12345391)*2654435769;
    i^=(i<<6)^(i>>26);
    i*=2654435769;
    i+=(i<<5)^(i>>12);
    seed ++;
    return float(max * i) * InverseMaxInt;
}

vec3 vec3rand(inout int seed, vec3 max) {
    return vec3(frand(seed, max.x), frand(seed, max.y), frand(seed, max.z));
}

void main() {
    vPosition.w = position.w-animationFactor;
    if(vPosition.w <= 0.0 && respawnParticles == 1.0) {
        int seed = gl_VertexID*int(1000*animationFactor);
        vPosition.xyz = vec3rand(seed, posRange)+posCenter;
        vPosition.w = frand(seed, lifeRange)+lifeCenter;
        vVelocity.xyz = normalize(vec3rand(seed, dirRange)+dirCenter)*length(dirRange)*0.5;
        vVelocity.w = frand(seed, sizeRange)+sizeCenter;
    }else{
        vPosition.xyz = position.xyz+velocity.xyz*animationFactor;
        vVelocity.xyz = velocity.xyz+velocityAdd;
        vVelocity.w = velocity.w;
    }
}