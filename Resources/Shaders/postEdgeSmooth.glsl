in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

out vec3 colorOut;
uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;

void main() {
	vec4 vDepthDiff;
	float depth = texture(sampler0, gl_FragCoord.xy).x;
	vDepthDiff.x = texture(sampler0, gl_FragCoord.xy+vec2( 0.0, 1.0)).x + texture(sampler0, gl_FragCoord.xy+vec2( 0.0,-1.0)).x;
	vDepthDiff.y = texture(sampler0, gl_FragCoord.xy+vec2( 1.0, 0.0)).x + texture(sampler0, gl_FragCoord.xy+vec2(-1.0, 0.0)).x;
	vDepthDiff.z = texture(sampler0, gl_FragCoord.xy+vec2( 1.0, 1.0)).x + texture(sampler0, gl_FragCoord.xy+vec2(-1.0,-1.0)).x;
	vDepthDiff.w = texture(sampler0, gl_FragCoord.xy+vec2( 1.0,-1.0)).x + texture(sampler0, gl_FragCoord.xy+vec2( 1.0,-1.0)).x;
	vDepthDiff = step(0.0, abs(vec4(depth*2.0) - vDepthDiff)*80.0 - vec4(0.5));
    
    float fT = max(dot(vDepthDiff, vec4(0.25)), 0.0);
	colorOut = texture(sampler1, gl_FragCoord.xy).xyz * (1.0-fT);
	colorOut += texture(sampler1, gl_FragCoord.xy+vec2( 0.0, 1.0)).xyz * fT;
	colorOut += texture(sampler1, gl_FragCoord.xy+vec2( 0.0,-1.0)).xyz * fT;
	colorOut += texture(sampler1, gl_FragCoord.xy+vec2( 1.0, 0.0)).xyz * fT;
	colorOut += texture(sampler1, gl_FragCoord.xy+vec2(-1.0, 0.0)).xyz * fT;
	colorOut *= 1.0/(1.0+fT*3.0);
}