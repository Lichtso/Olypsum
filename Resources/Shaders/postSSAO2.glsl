in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
out vec3 colorOut;

const float blurWidth = float(SSAO_QUALITY), blurInverse = 1.0/((blurWidth*2+1)*(blurWidth*2+1));

void main() {
    float occlusionValue = 0.0, occlusionSum = 0.0, depth = texture(sampler0, gl_FragCoord.xy).x+0.01;
    for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++) {
            float factor = step(texture(sampler0, gl_FragCoord.xy+vec2(x, y)).x, depth);
            occlusionValue += factor*texture(sampler1, gl_FragCoord.xy/SSAO_SCALE+vec2(x, y)).x;
            occlusionSum += factor;
        }
    colorOut = vec3(occlusionValue/occlusionSum);
    //colorOut = vec3((occlusionSum == 0.0) ? 1.0 : occlusionValue/occlusionSum);
}