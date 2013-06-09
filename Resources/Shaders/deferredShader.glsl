in vec2 position;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#separator

#if BLENDING_QUALITY == 1
out vec4 colorOut;
#else
out vec3 colorOut;
#endif
uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
uniform sampler2DRect sampler2;
uniform vec3 sceneAmbient;

void main() {
    colorOut.rgb = texture(sampler0, gl_FragCoord.xy).rgb; //Diffuse Light
    colorOut.rgb += sceneAmbient; //Ambient Light
    
    #if BLENDING_QUALITY > 0
    vec4 color = texture(sampler1, gl_FragCoord.xy);
    if(color.a == 0.0) discard;
    colorOut.rgb *= color.rgb; //Material Diffuse Color
    #if BLENDING_QUALITY == 1
    colorOut.a = color.a; //Material Diffuse Alpha
    #endif
    #else
    colorOut.rgb *= texture(sampler1, gl_FragCoord.xy).rgb; //Material Diffuse Color
    #endif
    
    colorOut.rgb += texture(sampler2, gl_FragCoord.xy).rgb; //Specular Light
}