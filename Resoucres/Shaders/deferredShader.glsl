attribute vec2 position;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
uniform sampler2DRect sampler2;
uniform sampler2DRect sampler3;
#if SSAO_QUALITY
uniform sampler2DRect sampler4;
uniform sampler2DRect sampler5;
#endif

void main() {
    vec3 color = texture2DRect(sampler0, gl_FragCoord.xy).rgb,
         diffuse = texture2DRect(sampler1, gl_FragCoord.xy).rgb+vec3(0.2),
         specular = texture2DRect(sampler2, gl_FragCoord.xy).rgb;
    float emission = texture2DRect(sampler3, gl_FragCoord.xy).b;
        
    #if SSAO_QUALITY
    vec2 uvPos;
    vec3 normal = texture2DRect(sampler4, gl_FragCoord.xy).xyz;
	float ambientOcclusion = 0.0, occlusionSum = 0.0;
    const float blurWidth = float(SSAO_QUALITY-1);
	for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++) {
            uvPos = gl_FragCoord.xy+vec2(x, y);
            if(dot(texture2DRect(sampler4, uvPos).xyz, normal) > 0.99) {
                occlusionSum += 1.0;
                ambientOcclusion += texture2DRect(sampler5, uvPos*0.5).x;
            }
        }
    
    if(occlusionSum > 0.0)
        ambientOcclusion *= 1.0 / occlusionSum;
    else
        ambientOcclusion = texture2DRect(sampler5, gl_FragCoord.xy*0.5).x;
    
    gl_FragData[0].rgb = color*diffuse*ambientOcclusion+color*emission+specular;
    #else
    gl_FragData[0].rgb = color*(diffuse+vec3(emission))+specular;
    #endif
    gl_FragData[0].a = 1.0;
}