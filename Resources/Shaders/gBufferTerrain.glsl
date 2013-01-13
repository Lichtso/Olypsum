in vec3 position;
in vec3 normal;
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
uniform vec3 textureScale;

out vec3 vPosition;
out vec3 vTexCoord;
out vec3 vNormal;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
	vPosition = (vec4(position, 1.0)*modelMat).xyz;
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

uniform sampler2DArray sampler0;
uniform sampler2DArray sampler1;

void main() {
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
}