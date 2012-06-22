attribute vec2 position;
varying vec2 vTexCoord;

void main() {
	gl_Position = vec4(position.x*2.0-1.0, position.y*2.0-1.0, 0.0, 1.0);
	vTexCoord = position;
}

#separator

varying vec2 vTexCoord;

#if PREPARE_BUFFERS

uniform sampler2D sampler0; //ColorBuffer
uniform sampler2D sampler1; //MaterialBuffer

void main() {
    gl_FragData[0].rgb = vec3(0.2);
    gl_FragData[0].a = 1.0;
    gl_FragData[1].rgb = texture2D(sampler0, vTexCoord).xyz*texture2D(sampler1, vTexCoord).b;
    gl_FragData[1].a = 1.0;
}

#else

uniform sampler2D sampler0; //ColorBuffer
uniform sampler2D sampler1; //DiffuseBuffer
uniform sampler2D sampler2; //SpecularBuffer

void main() {
    gl_FragData[0].rgb = texture2D(sampler0, vTexCoord).rgb*texture2D(sampler1, vTexCoord).rgb+texture2D(sampler2, vTexCoord).rgb;
    gl_FragData[0].a = 1.0;
}

#endif