attribute vec4 position;
attribute float velocity;
uniform float lifeInvMin;
uniform float lifeInvMax;
varying vec3 gPosition;
varying vec2 gLife;
varying float gSize;

void main() {
    gPosition = position.xyz;
    gLife.x = position.w*lifeInvMax;
    gLife.y = min(position.w*lifeInvMin, 1.0);
    gSize = velocity;
}

#separator

#extension GL_EXT_texture_array : require

#if TEXTURE_ANIMATION == 0
uniform sampler2D sampler0;
#else
uniform sampler2DArray sampler0;
#endif
varying vec3 vPosition;
varying vec4 vTexCoord;
varying vec3 vNormal;

void main() {
    #if TEXTURE_ANIMATION == 0 //2D texture
    gl_FragData[0] = texture2D(sampler0, vTexCoord.xy); //Color
    #else //3D texture
    float interpolZ = fract(vTexCoord.z);
    gl_FragData[0] = texture2DArray(sampler0, vec3(vTexCoord.xy, vTexCoord.z-0.5))*(1.0-interpolZ); //Color
    gl_FragData[0] += texture2DArray(sampler0, vec3(vTexCoord.xy, vTexCoord.z+0.5))*interpolZ;
    #endif
    gl_FragData[0].a *= vTexCoord.w;
    if(gl_FragData[0].a < 0.0039) discard;
    
    gl_FragData[1] = vec4(0.0, 0.0, 0.0, gl_FragData[0].a);
    gl_FragData[2] = vec4(vNormal, gl_FragData[0].a);
    gl_FragData[3] = vec4(vPosition, gl_FragData[0].a);
}

#separator

#parameter GL_GEOMETRY_VERTICES_OUT 4
#parameter GL_GEOMETRY_INPUT_TYPE GL_POINTS
#parameter GL_GEOMETRY_OUTPUT_TYPE GL_TRIANGLE_STRIP
#extension GL_EXT_geometry_shader4: enable

uniform mat4 modelViewMat;
uniform mat3 viewNormalMat;
varying in vec3 gPosition[1];
varying in vec2 gLife[1];
varying in float gSize[1];
varying out vec3 vPosition;
varying out vec4 vTexCoord;
varying out vec3 vNormal;

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