in vec3 position;
in vec3 normal;
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
uniform vec3 textureScale;
uniform float depthNear, depthFar;
uniform vec4 clipPlane[1];

out vec3 vPosition;
out vec3 vTexCoord;
out vec3 vNormal;
out float gl_ClipDistance[1];

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
    gl_Position.z = log2(max(gl_Position.w+depthNear, 0.5))*depthFar*gl_Position.w-gl_Position.w;
	vPosition = (vec4(position, 1.0)*modelMat).xyz;
    gl_ClipDistance[0] = dot(vec4(vPosition, 1.0), clipPlane[0]);
    vNormal = normal*normalMat;
    vTexCoord = position * textureScale;
    vTexCoord.z -= 0.5;
}

#separator

in vec3 vPosition;
in vec3 vTexCoord;
in vec3 vNormal;
out vec3 colorOut;
out vec3 materialOut;
out vec3 normalOut;
out vec3 positionOut;
out vec3 specularOut;

uniform float discardDensity;
uniform sampler2DArray sampler0;
uniform sampler2DArray sampler1;

#include random.glsl

void main() {
    if(vec1Vec2Rand(vTexCoord.st) > discardDensity) discard;
    
    float interpolZ = fract(vTexCoord.z+0.5), interpolZlow = 1.0-interpolZ;
    vec3 highCoord = vec3(vTexCoord.xy, vTexCoord.z+1.0);
    
    vec4 color = texture(sampler0, vTexCoord)*interpolZlow
                +texture(sampler0, highCoord)*interpolZ; //Color
    if(color.a < 0.0039) discard;
    colorOut = color.rgb;
    
    materialOut = texture(sampler1, vTexCoord).rgb*interpolZlow; //Material
    materialOut += texture(sampler1, highCoord).rgb*interpolZ;
    
    normalOut = normalize(vNormal); //Normal
	positionOut = vPosition; //Position
    specularOut = vec3(0.0); //Emission
}