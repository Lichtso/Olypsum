attribute vec3 position;

uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
}

#separator

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;

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
	float factor = max(0.0, texture2DRect(sampler0, gl_FragCoord.xy).x-0.85)*10.0;
    gl_FragData[0].rgb = vec3(0.0);
    const float blurWidth = float(DOF_QUALITY);
	for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++)
            gl_FragData[0].rgb += texture2DRect(sampler1, gl_FragCoord.xy+vec2(x, y)*factor).rgb;
    const float blurSum = 1.0 / ((blurWidth*2.0+1.0)*(blurWidth*2.0+1.0));
    gl_FragData[0].rgb *= blurSum;
    gl_FragData[0].a = 1.0;
}

#endif