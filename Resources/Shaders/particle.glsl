in vec4 position;
in vec4 velocity;
uniform float lifeInvMin;
uniform float lifeInvMax;
out vec3 gPosition;
out vec2 gLife;
out float gSize;

void main() {
    gPosition = position.xyz;
    gLife.x = position.w*lifeInvMax;
    gLife.y = min(position.w*lifeInvMin, 1.0);
    gSize = velocity.w;
}

#separator

#if TEXTURE_ANIMATION == 0
uniform sampler2D sampler0;
#else
uniform sampler2DArray sampler0;
#endif
uniform sampler2DRect sampler1;
in vec3 vPosition;
in vec4 vTexCoord;
in vec3 vNormal;
out vec4 colorOut;
out vec3 materialOut;
out vec3 normalOut;
out vec3 positionOut;
out vec3 specularOut;

void main() {
    #if TEXTURE_ANIMATION == 0 //2D texture
    colorOut = texture(sampler0, vTexCoord.xy); //Color
    #else //3D texture
    float interpolZ = fract(vTexCoord.z);
    colorOut = texture(sampler0, vec3(vTexCoord.xy, vTexCoord.z-0.5))*(1.0-interpolZ); //Color
    colorOut += texture(sampler0, vec3(vTexCoord.xy, vTexCoord.z+0.5))*interpolZ;
    #endif
    colorOut.a *= vTexCoord.w;
    if(colorOut.a < 0.0039) discard;
    
    materialOut = vec3(0.0, 0.0, 0.0);
    normalOut = vNormal;
    positionOut = vPosition;
    
    colorOut.rgb *= colorOut.a;
    #if BLENDING_QUALITY > 1 //Background lookup
    vec3 backgroundColor = (1.0-colorOut.a) * texture(sampler1, gl_FragCoord.xy).rgb;
    #if BLENDING_QUALITY == 2 //Correct Blending
    specularOut = backgroundColor;
    #elif BLENDING_QUALITY == 3 //Mixed Blending
    colorOut.rgb += backgroundColor;
    #endif //Mixed Blending
    #endif //Background lookup
}

#separator

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 modelViewMat;
uniform mat3 viewNormalMat;
in vec3 gPosition[1];
in vec2 gLife[1];
in float gSize[1];
out vec3 vPosition;
out vec4 vTexCoord;
out vec3 vNormal;

void main() {
    float size = gSize[0];
    vec3 rightVec = viewNormalMat[0]*size;
    vec3 upVec = viewNormalMat[1]*size;
    
    vNormal = viewNormalMat[2]-viewNormalMat[0]+viewNormalMat[1];
    vPosition = gPosition[0]-rightVec+upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec4(0.0, 0.0, gLife[0]);
	EmitVertex();
    
    vNormal = viewNormalMat[2]-viewNormalMat[0]-viewNormalMat[1];
    vPosition = gPosition[0]-rightVec-upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec4(0.0, 1.0, gLife[0]);
	EmitVertex();
    
    vNormal = viewNormalMat[2]+viewNormalMat[0]+viewNormalMat[1];
    vPosition = gPosition[0]+rightVec+upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec4(1.0, 0.0, gLife[0]);
	EmitVertex();
    
    vNormal = viewNormalMat[2]+viewNormalMat[0]-viewNormalMat[1];
    vPosition = gPosition[0]+rightVec-upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec4(1.0, 1.0, gLife[0]);
	EmitVertex();
}