attribute vec2 position;
uniform mat4 modelViewMat;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect sampler[1];
uniform float processingValue;

void main() {
    #if PROCESSING_TYPE == 1
    vec3 pos = vec3(0.0), posL = vec3(-1.0, 0.0, 0.0), posR = vec3(1.0, 0.0, 0.0),
         posB = vec3(0.0, -1.0, 0.0), posT = vec3(0.0, 1.0, 0.0);
    pos.z = texture2DRect(sampler[0], gl_FragCoord.xy).r*processingValue;
    posL.z = texture2DRect(sampler[0], gl_FragCoord.xy+posL.xy).r*processingValue;
    posR.z = texture2DRect(sampler[0], gl_FragCoord.xy+posR.xy).r*processingValue;
    posB.z = texture2DRect(sampler[0], gl_FragCoord.xy+posB.xy).r*processingValue;
    posT.z = texture2DRect(sampler[0], gl_FragCoord.xy+posT.xy).r*processingValue;
    posL -= pos; posR -= pos; posB -= pos; posT -= pos;
    vec3 normal = cross(posT, posL)+cross(posR, posT)+cross(posL, posB)+cross(posB, posR);
    gl_FragColor = vec4(normalize(normal)*0.5+vec3(0.5), pos.z);
    #elif PROCESSING_TYPE == 2
    gl_FragColor = vec4(0.0);
	const float blurSize = 2.0;
	for(float y = -blurSize; y <= blurSize; y += 1.0)
		for(float x = -blurSize; x <= blurSize; x += 1.0)
			gl_FragColor += texture2DRect(sampler[0], gl_FragCoord.xy+vec2(x, y)*processingValue);
	gl_FragColor /= (blurSize*2.0+1.0)*(blurSize*2.0+1.0);
    #endif
}