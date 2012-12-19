attribute vec3 position;
#if PROCESSING_TYPE == 3
attribute vec2 texCoord;
varying vec2 vTexCoord;
#elif PROCESSING_TYPE == 4
attribute vec3 color;
varying vec3 vColor;
#endif

uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
    #if PROCESSING_TYPE == 3
    vTexCoord = texCoord;
    #elif PROCESSING_TYPE == 4
    vColor = color;
    #endif
}

#separator

#extension GL_ARB_texture_rectangle : enable

#if PROCESSING_TYPE < 3
uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
#endif

#if PROCESSING_TYPE == 1

void main() {
	vec4 vDepthDiff;
	float depth = texture2DRect(sampler0, gl_FragCoord.xy).x;
	vDepthDiff.x = texture2DRect(sampler0, gl_FragCoord.xy+vec2( 0.0, 1.0)).x + texture2DRect(sampler0, gl_FragCoord.xy+vec2( 0.0,-1.0)).x;
	vDepthDiff.y = texture2DRect(sampler0, gl_FragCoord.xy+vec2( 1.0, 0.0)).x + texture2DRect(sampler0, gl_FragCoord.xy+vec2(-1.0, 0.0)).x;
	vDepthDiff.z = texture2DRect(sampler0, gl_FragCoord.xy+vec2( 1.0, 1.0)).x + texture2DRect(sampler0, gl_FragCoord.xy+vec2(-1.0,-1.0)).x;
	vDepthDiff.w = texture2DRect(sampler0, gl_FragCoord.xy+vec2( 1.0,-1.0)).x + texture2DRect(sampler0, gl_FragCoord.xy+vec2( 1.0,-1.0)).x;
	vDepthDiff = step(0.0, abs(vec4(depth*2.0) - vDepthDiff)*80.0 - vec4(0.5));
    
    float fT = max(dot(vDepthDiff, vec4(0.25)), 0.0);
	gl_FragData[0].rgb  = texture2DRect(sampler1, gl_FragCoord.xy).xyz * (1.0-fT);
	gl_FragData[0].rgb += texture2DRect(sampler1, gl_FragCoord.xy+vec2( 0.0, 1.0)).xyz * fT;
	gl_FragData[0].rgb += texture2DRect(sampler1, gl_FragCoord.xy+vec2( 0.0,-1.0)).xyz * fT;
	gl_FragData[0].rgb += texture2DRect(sampler1, gl_FragCoord.xy+vec2( 1.0, 0.0)).xyz * fT;
	gl_FragData[0].rgb += texture2DRect(sampler1, gl_FragCoord.xy+vec2(-1.0, 0.0)).xyz * fT;
	gl_FragData[0].rgb *= 1.0/(1.0+fT*3.0);
    gl_FragData[0].a = 1.0;
}

#elif PROCESSING_TYPE == 2

void main() {
	float factor = max(0.0, texture2DRect(sampler0, gl_FragCoord.xy).x-0.75)*10.0;
    gl_FragData[0].rgb = vec3(0.0);
    const float blurWidth = float(DOF_QUALITY);
	for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++)
            gl_FragData[0].rgb += texture2DRect(sampler1, gl_FragCoord.xy+vec2(x, y)*factor).rgb;
    const float blurSum = 1.0 / ((blurWidth*2.0+1.0)*(blurWidth*2.0+1.0));
    gl_FragData[0].rgb *= blurSum;
    gl_FragData[0].a = 1.0;
}

#elif PROCESSING_TYPE == 3
varying vec2 vTexCoord;
uniform float alpha;
uniform sampler2D sampler0;

void main() {
	gl_FragData[0] = texture2D(sampler0, vTexCoord.st);
    gl_FragData[0].a *= alpha;
    if(gl_FragData[0].a < 0.0039) discard;
}

#elif PROCESSING_TYPE == 4
varying vec3 vColor;

void main() {
	gl_FragData[0].rgb = vColor;
    gl_FragData[0].a = 1.0;
}

#endif