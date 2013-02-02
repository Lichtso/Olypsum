const float InverseMaxInt = 1.0 / 4294967295.0;

#ifdef GL_VERTEX_SHADER
int genSeed() {
    return gl_VertexID*int(1000*animationFactor);
}
#else
int genSeed() {
    int seed = (int(gl_FragCoord.x)^12345391)*2654435769;
    seed^=(seed<<6)^(seed>>26);
    seed*=int(gl_FragCoord.y);
    return seed+(seed<<5)^(seed>>12);
}
#endif

float frand(inout int seed, float max) {
    int i=(seed^12345391)*2654435769;
    i^=(i<<6)^(i>>26);
    i*=2654435769;
    i+=(i<<5)^(i>>12);
    seed ++;
    return float(max * i) * InverseMaxInt;
}

vec2 vec2rand(inout int seed, vec2 max) {
    return vec2(frand(seed, max.x), frand(seed, max.y));
}

vec3 vec3rand(inout int seed, vec3 max) {
    return vec3(frand(seed, max.x), frand(seed, max.y), frand(seed, max.z));
}