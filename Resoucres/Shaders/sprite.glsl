attribute vec3 position;
attribute vec3 texCoord;
attribute vec2 tangent;

uniform mat4 modelViewMat;
uniform mat4 viewMat;
varying vec3 vPosition;
varying vec3 vTexCoord;
varying vec3 vNormal;

void main() {
    //vPosition = position + normalize(viewMat[0].xyz)*tangent.x + normalize(viewMat[1].xyz)*tangent.y;
    vPosition = position + viewMat[0].xyz*tangent.x + viewMat[1].xyz*tangent.y;
	gl_Position = vec4(vPosition, 1.0)*modelViewMat;
    vNormal = viewMat[0].xyz*sign(tangent.x) + viewMat[1].xyz*sign(tangent.y) - viewMat[2].xyz;
    vTexCoord = texCoord;
}

#separator

uniform sampler2D sampler0; //ColorTexture
uniform vec3 color;
varying vec3 vPosition;
varying vec3 vTexCoord;
varying vec3 vNormal;

void main() {
	gl_FragData[0] = texture2D(sampler0, vTexCoord.st);
    gl_FragData[0].a *= 1.0 - vTexCoord.z;
    if(gl_FragData[0].a < 0.1) discard;
    if(color.r > -1.0) gl_FragData[0].rgb += color;
    
    gl_FragData[1].xyz = vPosition;
    gl_FragData[1].a = 1.0;
    gl_FragData[2].xyz = vNormal;
    gl_FragData[2].a = 1.0;
    gl_FragData[3].xyz = vec3(0.0);
    gl_FragData[3].a = 1.0;
}