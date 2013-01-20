in vec2 position;
uniform mat4 modelViewMat;

#if PROCESSING_TYPE == 3
out vec2 vPosition;
#endif

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    #if PROCESSING_TYPE == 3
    vPosition = position;
    #endif
}

#separator

#if PROCESSING_TYPE < 3
uniform sampler2DRect sampler0;
uniform float processingValue;
#endif

#if PROCESSING_TYPE == 1 //Normal Map Generator
out vec4 normalOut;

void main() {
    normalOut.a = texture(sampler0, gl_FragCoord.xy).r;
    vec3 pos = vec3(0.0), posL = vec3(-1.0, 0.0, 0.0), posR = vec3(1.0, 0.0, 0.0),
         posB = vec3(0.0, -1.0, 0.0), posT = vec3(0.0, 1.0, 0.0);
    pos.z = normalOut.a*processingValue;
    posL.z = texture(sampler0, gl_FragCoord.xy+posL.xy).r*processingValue;
    posR.z = texture(sampler0, gl_FragCoord.xy+posR.xy).r*processingValue;
    posB.z = texture(sampler0, gl_FragCoord.xy+posB.xy).r*processingValue;
    posT.z = texture(sampler0, gl_FragCoord.xy+posT.xy).r*processingValue;
    posL -= pos; posR -= pos; posB -= pos; posT -= pos;
    vec3 normal = normalize(cross(posT, posL)+cross(posR, posT)+cross(posL, posB)+cross(posB, posR));
    normalOut.rgb = vec3(vec2(0.5)-normal.xy*0.5, normal.z);
}

#elif PROCESSING_TYPE == 2 //Screen Blur
out vec3 colorOut;

const float blurWidth = 2.0, blurInverse = 1.0/((blurWidth*2+1)*(blurWidth*2+1));

void main() {
    colorOut = vec3(0.0);
	for(float y = -blurWidth; y <= blurWidth; y += 1.0)
		for(float x = -blurWidth; x <= blurWidth; x += 1.0)
			colorOut += texture(sampler0, gl_FragCoord.xy+vec2(x, y)*processingValue).rgb;
	colorOut *= blurInverse;
}

#elif PROCESSING_TYPE == 3 //Spot Light Circle Mask
in vec2 vPosition;

void main() {
    if(length(vPosition) < 0.99) discard;
}

#elif PROCESSING_TYPE == 4 //SSAO combine
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

#endif