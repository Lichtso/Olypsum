attribute vec3 position;
attribute vec3 normal;
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
uniform vec3 textureScale;

varying vec3 vPosition;
varying vec3 vTexCoord;
varying vec3 vNormal;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
	vPosition = (vec4(position, 1.0)*modelMat).xyz;
    vNormal = normal*normalMat;
    vTexCoord = position * textureScale;
    vTexCoord.z -= 0.5;
}

#separator

#extension GL_EXT_texture_array : require

varying vec3 vPosition;
varying vec3 vTexCoord;
varying vec3 vNormal;

uniform sampler2DArray sampler0;
uniform sampler2DArray sampler1;

void main() {
    float interpolZ = fract(vTexCoord.z+0.5), interpolZlow = 1.0-interpolZ;
    vec3 highCoord = vec3(vTexCoord.xy, vTexCoord.z+1.0);
    
    gl_FragData[0] = texture2DArray(sampler0, vTexCoord)*interpolZlow; //Color
    gl_FragData[0] += texture2DArray(sampler0, highCoord)*interpolZ;
    
    gl_FragData[1] = texture2DArray(sampler1, vTexCoord)*interpolZlow; //Material
    gl_FragData[1] += texture2DArray(sampler1, highCoord)*interpolZ;
    
    gl_FragData[2] = vec4(normalize(vNormal), 1.0); //Normal
	gl_FragData[3] = vec4(vPosition, 1.0); //Position
}