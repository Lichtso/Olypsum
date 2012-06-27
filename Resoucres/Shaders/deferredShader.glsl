attribute vec2 position;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect sampler[4];

void main() {
    vec3 color = texture2DRect(sampler[0], gl_FragCoord.xy).rgb,
         material = texture2DRect(sampler[1], gl_FragCoord.xy).rgb,
         diffuse = texture2DRect(sampler[2], gl_FragCoord.xy).rgb,
         specular = texture2DRect(sampler[3], gl_FragCoord.xy).rgb;
    gl_FragData[0].rgb = color*(diffuse+vec3(material.b+0.2))+specular;
    gl_FragData[0].a = 1.0;
}