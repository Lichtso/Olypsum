attribute vec4 position;
attribute float velocity;
uniform float lifeMin;
varying vec3 gPosition;
varying float gAlpha;
varying float gSize;

void main() {
    gPosition = position.xyz;
    gAlpha = min(position.w*lifeMin, 1.0);
    gSize = velocity;
}

#separator

uniform sampler2D sampler0;
varying vec3 vPosition;
varying vec3 vTexCoord;
varying vec3 vNormal;

void main() {
    gl_FragData[0] = texture2D(sampler0, vTexCoord.xy);
    gl_FragData[0].a *= vTexCoord.z;
    if(gl_FragData[0].a < 0.0039) discard;
    
    gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
    gl_FragData[2] = vec4(vNormal, 1.0);
    gl_FragData[3] = vec4(vPosition, 1.0);
}

#separator

#parameter GL_GEOMETRY_VERTICES_OUT 4
#parameter GL_GEOMETRY_INPUT_TYPE GL_POINTS
#parameter GL_GEOMETRY_OUTPUT_TYPE GL_TRIANGLE_STRIP
#extension GL_EXT_geometry_shader4: enable

uniform mat4 modelViewMat;
uniform mat3 viewNormalMat;
varying in vec3 gPosition[1];
varying in float gAlpha[1];
varying in float gSize[1];
varying out vec3 vPosition;
varying out vec3 vTexCoord;
varying out vec3 vNormal;

void main() {
    float size = gSize[0];
    vec3 rightVec = viewNormalMat[0]*size;
    vec3 upVec = viewNormalMat[1]*size;
    
    vNormal = viewNormalMat[2]-viewNormalMat[0]+viewNormalMat[1];
    vPosition = gPosition[0]-rightVec+upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec3(0.0, 0.0, gAlpha[0]);
	EmitVertex();
    
    vNormal = viewNormalMat[2]-viewNormalMat[0]-viewNormalMat[1];
    vPosition = gPosition[0]-rightVec-upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec3(0.0, 1.0, gAlpha[0]);
	EmitVertex();
    
    vNormal = viewNormalMat[2]+viewNormalMat[0]+viewNormalMat[1];
    vPosition = gPosition[0]+rightVec+upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec3(1.0, 0.0, gAlpha[0]);
	EmitVertex();
    
    vNormal = viewNormalMat[2]+viewNormalMat[0]-viewNormalMat[1];
    vPosition = gPosition[0]+rightVec-upVec;
    gl_Position = vec4(vPosition.xyz, 1.0)*modelViewMat;
	vTexCoord = vec3(1.0, 1.0, gAlpha[0]);
	EmitVertex();
}