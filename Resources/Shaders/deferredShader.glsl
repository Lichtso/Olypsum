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
uniform vec3 sceneAmbient;

void main() {
    colorOut = texture(sampler0, gl_FragCoord.xy).rgb; //Diffuse Light
    colorOut += sceneAmbient; //Ambient Light
    
    #if BLENDING_QUALITY > 0
    vec4 color = texture(sampler1, gl_FragCoord.xy);
    if(color.a == 0.0) discard;
    colorOut *= color.rgb; //Material Diffuse Color
    #else
    colorOut *= texture(sampler1, gl_FragCoord.xy).rgb; //Material Diffuse Color
    #endif
    
    colorOut += texture(sampler2, gl_FragCoord.xy).rgb; //Specular Light
    
    #if BLENDING_QUALITY == 1
    colorOut += texture(sampler3, gl_FragCoord.xy).rgb*(1.0-color.a);
    #endif
}