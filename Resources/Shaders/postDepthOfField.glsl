in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

out vec3 colorOut;
uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
uniform vec3 fogColor;
uniform float fogDistance;

const float blurWidth = float(DOF_QUALITY), blurInverse = 1.0/((blurWidth*2+1)*(blurWidth*2+1));

void main() {
	float factor = clamp(texture(sampler0, gl_FragCoord.xy).x*0.5, 0.0, 1.0);
    colorOut = vec3(0.0);
	for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++)
            colorOut += texture(sampler1, gl_FragCoord.xy+vec2(x, y)*factor*3.0).rgb;
    colorOut *= blurInverse;
    
    #if FOG_ACTIVE == 1
    colorOut = mix(colorOut, fogColor, clamp(factor*factor*fogDistance, 0.0, 1.0));
    #endif
}