attribute vec2 position;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#separator

#extension GL_ARB_texture_rectangle : enable

#if SSAO_QUALITY
uniform sampler2DRect sampler[6];
#else
uniform sampler2DRect sampler[4];
#endif

void main() {
    vec3 color = texture2DRect(sampler[0], gl_FragCoord.xy).rgb,
         material = texture2DRect(sampler[1], gl_FragCoord.xy).rgb,
         diffuse = texture2DRect(sampler[2], gl_FragCoord.xy).rgb,
         specular = texture2DRect(sampler[3], gl_FragCoord.xy).rgb;
    
    #if SSAO_QUALITY
    vec2 uvPos;
    vec3 normal = texture2DRect(sampler[4], gl_FragCoord.xy).xyz;
	float ambientOcclusion = 0.0, occlusionSum = 0.0;
    const float blurWidth = float(SSAO_QUALITY);
	for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++) {
            uvPos = gl_FragCoord.xy+vec2(x, y);
            if(dot(texture2DRect(sampler[4], uvPos).xyz, normal) > 0.99) {
                occlusionSum += 1.0;
                ambientOcclusion += texture2DRect(sampler[5], uvPos).x;
            }
        }
    
    if(occlusionSum > 0.0)
        ambientOcclusion *= 1.0 / occlusionSum;
    else
        ambientOcclusion = texture2DRect(sampler[5], gl_FragCoord.xy).x;
    
    gl_FragData[0].rgb = color*(material.b+0.2)+(color*diffuse+specular)*ambientOcclusion;
    #else
    gl_FragData[0].rgb = color*(diffuse+vec3(material.b+0.2))+specular;
    #endif
    gl_FragData[0].a = 1.0;
}