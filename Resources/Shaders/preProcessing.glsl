attribute vec2 position;
uniform mat4 modelViewMat;

#if PROCESSING_TYPE == 3
varying vec2 vPosition;
#endif

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    #if PROCESSING_TYPE == 3
    vPosition = position;
    #endif
}

#separator

#extension GL_ARB_texture_rectangle : enable

#if PROCESSING_TYPE < 3
uniform sampler2DRect sampler0;
uniform float processingValue;
#endif

#if PROCESSING_TYPE == 1

void main() {
    gl_FragColor.a = texture2DRect(sampler0, gl_FragCoord.xy).r;
    vec3 pos = vec3(0.0), posL = vec3(-1.0, 0.0, 0.0), posR = vec3(1.0, 0.0, 0.0),
         posB = vec3(0.0, -1.0, 0.0), posT = vec3(0.0, 1.0, 0.0);
    pos.z = gl_FragColor.a*processingValue;
    posL.z = texture2DRect(sampler0, gl_FragCoord.xy+posL.xy).r*processingValue;
    posR.z = texture2DRect(sampler0, gl_FragCoord.xy+posR.xy).r*processingValue;
    posB.z = texture2DRect(sampler0, gl_FragCoord.xy+posB.xy).r*processingValue;
    posT.z = texture2DRect(sampler0, gl_FragCoord.xy+posT.xy).r*processingValue;
    posL -= pos; posR -= pos; posB -= pos; posT -= pos;
    vec3 normal = normalize(cross(posT, posL)+cross(posR, posT)+cross(posL, posB)+cross(posB, posR));
    gl_FragColor.rgb = vec3(vec2(0.5)-normal.xy*0.5, normal.z);
}

#elif PROCESSING_TYPE == 2

void main() {
    gl_FragColor = vec4(0.0);
	const float blurSize = 2.0;
	for(float y = -blurSize; y <= blurSize; y += 1.0)
		for(float x = -blurSize; x <= blurSize; x += 1.0)
			gl_FragColor += texture2DRect(sampler0, gl_FragCoord.xy+vec2(x, y)*processingValue);
	gl_FragColor /= (blurSize*2.0+1.0)*(blurSize*2.0+1.0);
}

#elif PROCESSING_TYPE == 3
varying vec2 vPosition;

void main() {
    if(length(vPosition) < 0.99) discard;
}

#endif