in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

out vec4 colorOut;
uniform vec2 size, edgeGradient;
uniform float cornerRadius, seed;
uniform vec4 topColor, bottomColor, borderColor;
uniform bool transposed, roundedCorners[4];

#include random.glsl

void main() {
    vec2 cornerVecs[4];
    cornerVecs[0] = vec2(cornerRadius-gl_FragCoord.x, cornerRadius-gl_FragCoord.y),
    cornerVecs[1] = vec2(gl_FragCoord.x-size.x+cornerRadius, cornerRadius-gl_FragCoord.y),
    cornerVecs[2] = vec2(cornerRadius-gl_FragCoord.x, gl_FragCoord.y-size.y+cornerRadius),
    cornerVecs[3] = vec2(gl_FragCoord.x-size.x+cornerRadius, gl_FragCoord.y-size.y+cornerRadius);
    float cornerDist = cornerRadius-min(min(gl_FragCoord.x, size.x-gl_FragCoord.x), min(gl_FragCoord.y, size.y-gl_FragCoord.y));
    
    for(int i = 0; i < 4; i ++)
        if(roundedCorners[i] && cornerVecs[i].x > 0.0 && cornerVecs[i].y > 0.0) {
            cornerDist = length(cornerVecs[i]);
            if(cornerDist > cornerRadius) {
                colorOut.a = 0.0;
                return;
            }
        }
    
    if(cornerDist > cornerRadius-1.0) {
        colorOut = borderColor;
        return;
    }
    
    colorOut = mix(topColor, bottomColor, (transposed) ? gl_FragCoord.x/size.x : gl_FragCoord.y/size.y);
    colorOut.rgb *= mix(edgeGradient.s, edgeGradient.t, max(cornerDist/cornerRadius, 0.0));
    #if DECORATION_TYPE == 1
    colorOut.rgb *= 0.9+0.1*vec1Vec3Noise(vec3(gl_FragCoord.x*0.01, seed, gl_FragCoord.y*0.8));
    #elif DECORATION_TYPE == 2
    colorOut.rgb *= clamp(0.5+2.0*length(fract(vec2(gl_FragCoord.y*0.1+gl_FragCoord.x*0.2, gl_FragCoord.y*0.2-gl_FragCoord.x*0.1))-vec2(0.5)), 0.0, 1.0);
    #endif
}