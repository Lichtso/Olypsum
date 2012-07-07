attribute vec2 position;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#separator

#extension GL_ARB_texture_rectangle : enable

#if SSAO_ENABLED
uniform sampler2DRect sampler[6];
#else
uniform sampler2DRect sampler[4];
#endif

void main() {
    vec3 color = texture2DRect(sampler[0], gl_FragCoord.xy).rgb,
         material = texture2DRect(sampler[1], gl_FragCoord.xy).rgb,
         diffuse = texture2DRect(sampler[2], gl_FragCoord.xy).rgb,
         specular = texture2DRect(sampler[3], gl_FragCoord.xy).rgb;
    
    #if SSAO_ENABLED
    float depth = texture2DRect(sampler[4], gl_FragCoord.xy).x*1.02;
	float ambientOcclusion = 0.0, occlusionSum = 0.0;
	for(float x = -2.0; x <= 2.0; x ++)
        for(float y = -2.0; y <= 2.0; y ++) {
            vec2 uvPos = gl_FragCoord.xy+vec2(x, y);
            float vOcc = texture2DRect(sampler[5], uvPos).x;
            float occlusionActive = step(texture2DRect(sampler[4], uvPos).x, depth);
            occlusionSum += occlusionActive;
            ambientOcclusion += vOcc*occlusionActive;
        }
    ambientOcclusion *= 1.0 / occlusionSum;
    
    //float ambientOcclusion = texture2DRect(sampler[5], gl_FragCoord.xy).x;
    gl_FragData[0].rgb = color*(ambientOcclusion);
    //gl_FragData[0].rgb = color*(material.b+0.2)+(color*diffuse+specular)*ambientOcclusion;
    #else
    gl_FragData[0].rgb = color*(diffuse+vec3(material.b+0.2))+specular;
    #endif
    gl_FragData[0].a = 1.0;
}