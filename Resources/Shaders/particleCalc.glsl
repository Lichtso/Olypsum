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

#include random.glsl

void main() {
    vPosition.w = position.w-animationFactor;
    if(vPosition.w <= 0.0 && respawnParticles == 1.0) {
        int seed = genSeed();
        vPosition.xyz = vec3SeedRand(seed, posRange)+posCenter;
        vPosition.w = vec1SeedRand(seed, lifeRange)+lifeCenter;
        vVelocity.xyz = normalize(vec3SeedRand(seed, dirRange)+dirCenter)*length(dirRange)*0.5;
        vVelocity.w = vec1SeedRand(seed, sizeRange)+sizeCenter;
    }else{
        vPosition.xyz = position.xyz+velocity.xyz*animationFactor;
        vVelocity.xyz = velocity.xyz+velocityAdd;
        vVelocity.w = velocity.w;
    }
}