attribute vec3 position;
attribute vec3 texCoord;
attribute vec2 tangent;

uniform mat4 modelViewMat;
uniform mat4 viewMat;
varying vec3 vPosition;
varying vec3 vTexCoord;
varying vec3 vNormal;

void main() {
    vec3 xDir = normalize(viewMat[0].xyz), yDir = normalize(viewMat[1].xyz);
    vPosition = position + xDir*tangent.x + yDir*tangent.y;
	gl_Position = vec4(vPosition, 1.0)*modelViewMat;
    vNormal = xDir*sign(tangent.x) + yDir*sign(tangent.y) - viewMat[2].xyz;
    vTexCoord = texCoord;
}

#separator

uniform sampler2D sampler[1];
uniform vec3 color;
varying vec3 vPosition;
varying vec3 vTexCoord;
varying vec3 vNormal;

void main() {
	gl_FragData[0] = texture2D(sampler[0], vTexCoord.st);
    gl_FragData[0].a *= 1.0 - vTexCoord.z;
    if(0.1 > gl_FragData[0].a) discard;
    if(color.r > -1.0) gl_FragData[0].rgb += color;
    
    gl_FragData[1].xyz = vNormal;
    gl_FragData[1].a = 1.0;
    gl_FragData[2].xyz = vec3(0.0);
    gl_FragData[2].a = 1.0;
    #if POSITION_BUFFER
    gl_FragData[3].rgb = vPosition;
    gl_FragData[3].a = 1.0;
    #endif
}