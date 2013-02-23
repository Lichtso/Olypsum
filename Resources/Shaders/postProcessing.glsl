in vec3 position;
#if PROCESSING_TYPE == 3
uniform mat3 textureMat;
out vec2 vTexCoord;
#elif PROCESSING_TYPE == 4
uniform mat4 modelMat;
out vec3 vPosition;
#endif

uniform mat4 modelViewMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
    #if PROCESSING_TYPE == 3
    vTexCoord = (vec3(position.xy, 1.0)*textureMat).xy;
    #elif PROCESSING_TYPE == 4
    vPosition = (vec4(position, 1.0)*modelMat).xyz;
    #endif
}

#separator

#if PROCESSING_TYPE < 3
uniform sampler2DRect sampler0;
uniform sampler2DRect sampler1;
#endif

#if PROCESSING_TYPE == 1 //Edge Smooth
out vec3 colorOut;

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

#elif PROCESSING_TYPE == 2 //Depth of Field
out vec3 colorOut;

const float blurWidth = float(DOF_QUALITY), blurInverse = 1.0/((blurWidth*2+1)*(blurWidth*2+1));

void main() {
	float factor = clamp(texture(sampler0, gl_FragCoord.xy).x*3.0-1.8, 0.0, 3.0);
    colorOut = vec3(0.0);
	for(float x = -blurWidth; x <= blurWidth; x ++)
        for(float y = -blurWidth; y <= blurWidth; y ++)
            colorOut += texture(sampler1, gl_FragCoord.xy+vec2(x, y)*factor).rgb;
    colorOut *= blurInverse;
}

#elif PROCESSING_TYPE == 3 //Sprites
in vec2 vTexCoord;
out vec4 colorOut;
uniform float alpha;
uniform sampler2D sampler0;

void main() {
	colorOut = texture(sampler0, vTexCoord.st);
    colorOut.a *= alpha;
    if(colorOut.a < 0.0039) discard;
}

#elif PROCESSING_TYPE == 4 //Debug draw colored
uniform vec3 color;
in vec3 vPosition;
out vec3 colorOut;
out vec3 materialOut;
out vec3 normalOut;
out vec3 positionOut;

void main() {
	colorOut = color;
    materialOut = vec3(0.0, 0.0, 0.0);
    normalOut = vec3(0.0);
    positionOut = vPosition;
}

#endif