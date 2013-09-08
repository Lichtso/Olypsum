in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
out vec3 colorOut;

const float blurWidth = float(SSAO_QUALITY-1), blurInverse = 1.0/((blurWidth*2+1)*(blurWidth*2+1));

void main() {
    float ambientOcclusion = 0.0, depth = 1.0/texture(sampler0, gl_FragCoord.xy).x;
    for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++)
            ambientOcclusion += texture(sampler1, gl_FragCoord.xy/SSAO_SCALE+vec2(x, y)).x *
                                step(0.995, texture(sampler0, gl_FragCoord.xy+vec2(x, y)*3.5).x*depth);
    
    colorOut = vec3(ambientOcclusion*blurInverse);
}