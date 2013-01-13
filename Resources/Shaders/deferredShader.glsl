in vec2 position;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#separator

out vec3 colorOut;
uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
uniform sampler2DRect sampler2;
uniform sampler2DRect sampler3;
uniform sampler2DRect sampler4;

void main() {
    #if BLENDING_QUALITY
    vec4 color = texture(sampler0, gl_FragCoord.xy);
    if(color.a == 0.0) discard;
    #if BLENDING_QUALITY == 1
    color.rgb *= color.a;
    color.rgb += texture(sampler4, gl_FragCoord.xy).rgb*(1.0-color.a);
    #endif
    #else
    vec3 color = texture(sampler0, gl_FragCoord.xy).rgb;
    #endif
    vec3 diffuse = texture(sampler1, gl_FragCoord.xy).rgb+vec3(0.2),
         specular = texture(sampler2, gl_FragCoord.xy).rgb;
    float emission = texture(sampler3, gl_FragCoord.xy).b;
    
    colorOut = color.rgb*(diffuse+vec3(emission))+specular;
}