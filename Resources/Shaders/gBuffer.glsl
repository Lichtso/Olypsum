in vec3 position;
in vec2 texCoord;
in vec3 normal;
#if SKELETAL_ANIMATION
in vec3 weights;
in vec3 joints;

uniform mat4 jointMats[64];
uniform mat4 viewMat;
#else
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
#endif
uniform vec4 clipPlane[1];

#if BUMP_MAPPING > 0
out vec3 gPosition;
out vec2 gTexCoord;
out vec3 gNormal;
#else
out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;
out float gl_ClipDistance[1];
#endif

void main() {
    #if SKELETAL_ANIMATION
    mat4 mat = mat4(0.0);
    mat += weights[0]*jointMats[int(joints[0])];
    mat += weights[1]*jointMats[int(joints[1])];
    mat += weights[2]*jointMats[int(joints[2])];
    vec4 pos = vec4(position, 1.0) * mat;
    gl_Position = pos * viewMat;
    #if BUMP_MAPPING
    gPosition = pos.xyz/pos.w;
    gNormal = normalize((vec4(normal, 0.0) * mat).xyz);
    gTexCoord = texCoord;
    #else
    vPosition = pos.xyz/pos.w;
    vNormal = normalize((vec4(normal, 0.0) * mat).xyz);
    vTexCoord = texCoord;
    gl_ClipDistance[0] = dot(vec4(vPosition, 1.0), clipPlane[0]);
    #endif
    #else
    gl_Position = vec4(position, 1.0)*modelViewMat;
    #if BUMP_MAPPING
    gPosition = (vec4(position, 1.0)*modelMat).xyz;
    gNormal = normal*normalMat;
    gTexCoord = texCoord;
    #else
	vPosition = (vec4(position, 1.0)*modelMat).xyz;
    vNormal = normal*normalMat;
    vTexCoord = texCoord;
    gl_ClipDistance[0] = dot(vec4(vPosition, 1.0), clipPlane[0]);
    #endif
    #endif
}

#separator

#define M_PI 3.14159265358979323846

in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;
#if BUMP_MAPPING > 0
in vec3 vTangent;
in vec3 vBitangent;
#endif
out vec4 colorOut;
out vec3 materialOut;
out vec3 normalOut;
out vec3 positionOut;
out vec3 specularOut;

#if TEXTURE_ANIMATION == 0
uniform sampler2D sampler0;
#else
uniform vec2 texCoordAnimF;
uniform vec2 texCoordAnimZ;
uniform sampler2DArray sampler0;
#endif
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2DRect sampler3;
uniform float discardDensity;
uniform vec3 camPos;
uniform mat3 viewNormalMat;

#if BLENDING_QUALITY > 0 && BUMP_MAPPING == 2
const int maxWaves = 4;
uniform float waveLen[maxWaves];
uniform float waveAge[maxWaves];
uniform float waveAmp[maxWaves];
uniform vec2 waveOri[maxWaves];
#endif

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void setColor(vec2 texCoord) {
    #if TEXTURE_ANIMATION == 0 //2D texture
    colorOut = texture(sampler0, texCoord); //Color
    #else //3D texture
    colorOut = texture(sampler0, vec3(texCoord, texCoordAnimZ.x))*texCoordAnimF.x
                +texture(sampler0, vec3(texCoord, texCoordAnimZ.y))*texCoordAnimF.y; //Color
    #endif
    materialOut = texture(sampler1, texCoord).rgb;
    if(colorOut.a < 0.0039) discard;
}

#if BUMP_MAPPING == 3
void LinearParallax(inout vec3 texCoord, vec3 viewVec, float steps) {
	for(float i = 0.0; i < steps; i ++) {
		if(texCoord.z >= texture(sampler2, texCoord.xy).a)
            return;
        texCoord += viewVec;
    }
}

void BinaryParallax(inout vec3 texCoord, vec3 viewVec) {
	for(int i = 0; i < 6; i ++) {
		if(texCoord.z < texture(sampler2, texCoord.xy).a)
            texCoord += viewVec;
		viewVec *= 0.5;
		texCoord -= viewVec;
	}
}
#endif

void main() {
    if(random(gl_FragCoord.xy) > discardDensity) discard;
    
    #if BUMP_MAPPING <= 1 || BLENDING_QUALITY > 0
    setColor(vTexCoord);
    #endif
    normalOut = normalize(vNormal);
    positionOut = vPosition;
    
    #if BLENDING_QUALITY == 0 //Solid
    
    #if BUMP_MAPPING == 1 //Normal mapping
    vec3 bumpMap = texture(sampler2, vTexCoord).xyz;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    normalOut = mat3(vTangent, vBitangent, normalOut)*bumpMap;
    #elif BUMP_MAPPING > 1 //Parallax
    
    vec3 viewVec = normalize(camPos-vPosition);
    #if BUMP_MAPPING == 2 //Parallax simple
    viewVec.xy = vec2(dot(viewVec, vTangent), dot(viewVec, vBitangent));
    vec2 texCoord = vTexCoord-viewVec.xy*texture(sampler2, vTexCoord).a*0.07;
    #elif BUMP_MAPPING == 3 //Parallax occlusion
    viewVec = vec3(dot(viewVec, vTangent), dot(viewVec, vBitangent), dot(viewVec, normalOut));
    float steps = floor((1.0 - viewVec.z) * 18.0) + 2.0;
    viewVec.xy *= -0.04/viewVec.z;
    viewVec.z = 1.0;
    viewVec /= steps;
    vec3 texCoord = vec3(vTexCoord, 0.0);
    LinearParallax(texCoord, viewVec, steps);
	BinaryParallax(texCoord, viewVec);
    #endif
    setColor(texCoord.xy);
    //if(abs(texCoord.x-0.5) > 0.5 || abs(texCoord.y-0.5) > 0.5) discard;
    gl_FragDepth = gl_FragCoord.z+length(texCoord.xy-vTexCoord)*0.05; //Depth
    vec3 bumpMap = texture(sampler2, texCoord.xy).xyz;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    normalOut = mat3(vTangent, vBitangent, normalOut)*bumpMap;
    #endif //Parallax
    
    #else //Transparent
    
    #if BUMP_MAPPING == 1 //Glass refraction
    vec3 bumpMap = texture(sampler2, vTexCoord).xyz;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    normalOut = mat3(vTangent, vBitangent, normalOut)*bumpMap;
    #elif BUMP_MAPPING == 2 //Water waves refraction
    vec3 bumpMap = vec3(0.0);
    for(int i = 0; i < maxWaves; i ++) {
        //if(waveAmp[i] <= 0.0) continue; //UNKNOWN PERFORMANCE USE
        vec2 diff = waveOri[i]-vTexCoord;
        float len = length(diff);
        diff /= len;
        len = sin(clamp(len*waveLen[i]-waveAge[i], 0.0, M_PI*2.0))*0.5+0.5;
        bumpMap.xy += diff*len*waveAmp[i];
        bumpMap.xy -= diff*(1.0-len)*waveAmp[i];
    }
    bumpMap.z = 1.0;
    normalOut = mat3(vTangent, vBitangent, normalOut)*normalize(bumpMap);
    #endif //Water waves
    
    #if BLENDING_QUALITY > 1 //Background lookup
    #if BUMP_MAPPING == 0 //No refraction
    texture(sampler2, vTexCoord); //Place holder
    #endif //No refraction
    specularOut = (1.0-colorOut.a) * texture(sampler3, gl_FragCoord.xy+(viewNormalMat*normalOut).xy*10.0).rgb;
    #endif //Background lookup
    
    colorOut.rgb *= colorOut.a;
    #endif //Transparent
}

#separator

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec4 clipPlane[1];
in vec3 gPosition[3];
in vec2 gTexCoord[3];
in vec3 gNormal[3];
out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out float gl_ClipDistance[1];

void main() {
	vec3 posBA = gPosition[1]-gPosition[0], posCA = gPosition[2]-gPosition[0];
	vec2 texBA = gTexCoord[1]-gTexCoord[0], texCA = gTexCoord[2]-gTexCoord[0];
	vec3 tangent = normalize((texCA.t*posBA-texBA.t*posCA) / (texBA.s*texCA.t-texBA.t*texCA.s)),
		 bitangent = normalize((texCA.s*posBA-texBA.s*posCA) / (texBA.t*texCA.s-texBA.s*texCA.t));
    
    for(int i = 0; i < 3; i ++) {
		gl_Position = gl_in[i].gl_Position;
        gl_ClipDistance[0] = dot(vec4(gPosition[i], 1.0), clipPlane[0]);
		vPosition = gPosition[i];
		vTexCoord = gTexCoord[i];
		vNormal = gNormal[i];
		vTangent = tangent;
		vBitangent = bitangent;
		EmitVertex();
	}
}