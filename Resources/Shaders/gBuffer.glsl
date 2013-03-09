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
#if REFLECTION_TYPE == 1
uniform sampler2DRect sampler4;
#elif REFLECTION_TYPE == 2
uniform samplerCube sampler4;
#endif

uniform float discardDensity;
uniform vec3 camPos;
uniform mat3 viewNormalMat;

#include random.glsl

void setColor(vec2 texCoord) {
    #if TEXTURE_ANIMATION == 0 //2D texture
    colorOut = texture(sampler0, texCoord); //Color
    #else //3D texture
    colorOut = texture(sampler0, vec3(texCoord, texCoordAnimZ.x))*texCoordAnimF.x
              +texture(sampler0, vec3(texCoord, texCoordAnimZ.y))*texCoordAnimF.y; //Color
    #endif
    if(colorOut.a < 0.0039) discard;
    materialOut = texture(sampler1, texCoord).rgb; //Material
    specularOut = colorOut.rgb*materialOut.b; //Emission
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
    if(vec1Vec2Rand(vTexCoord.st) > discardDensity) discard;
    
    #if BUMP_MAPPING != 2 && BUMP_MAPPING != 3 //No parallax
    setColor(vTexCoord);
    #endif //No parallax
    
    vec3 viewVec = normalize(camPos-vPosition);
    normalOut = normalize(vNormal);
    
    #if BUMP_MAPPING >= 2 && BUMP_MAPPING <= 3 //Parallax
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
    #endif //Parallax occlusion
    setColor(texCoord.xy);
    //if(abs(texCoord.x-0.5) > 0.5 || abs(texCoord.y-0.5) > 0.5) discard;
    gl_FragDepth = gl_FragCoord.z+length(texCoord.xy-vTexCoord)*0.05; //Depth
    vec3 modelNormal = texture(sampler2, texCoord.xy).xyz;
    normalOut.xy = modelNormal.xy*2.0-vec2(1.0);
    #endif //Parallax
    
    #if BUMP_MAPPING > 0
    #if BUMP_MAPPING == 1 //Normal mapping
    vec3 modelNormal = texture(sampler2, vTexCoord).xyz;
    modelNormal.xy = modelNormal.xy*2.0-vec2(1.0);
    #elif BUMP_MAPPING == 4 //Normal noise (water)
    vec3 modelNormal;
    modelNormal.x = vec1Vec3Noise(vec3(vTexCoord.s, vTexCoord.t, texCoordAnimZ.x));
    modelNormal.y = vec1Vec3Noise(vec3(vTexCoord.s+22454.0, vTexCoord.t+6314.0, texCoordAnimZ.x+1373.0));
    modelNormal = normalize(vec3(modelNormal.xy, 1.0));
    #endif
    normalOut = mat3(vTangent, vBitangent, normalOut) * modelNormal;
    #endif
    positionOut = vPosition;
    
    #if BUMP_MAPPING == 0 || BUMP_MAPPING == 4 //No normal mapping
    texture(sampler2, vTexCoord); //Place holder
    #endif
    
    #if BLENDING_QUALITY == 0 //No transparent
    texture(sampler3, gl_FragCoord.xy); //Place holder
    #elif BLENDING_QUALITY > 1 //Transparent
    #if BUMP_MAPPING == 0 //No normal mapping
    specularOut += (1.0-colorOut.a) * texture(sampler3, gl_FragCoord.xy).rgb;
    #else //Normal mapping
    specularOut += (1.0-colorOut.a) * texture(sampler3, gl_FragCoord.xy+modelNormal.xy*20.0).rgb;
    #endif //Normal mapping
    #endif //Transparent
    
    #if REFLECTION_TYPE == 1 //Plane Reflection
    #if BUMP_MAPPING == 0 //No normal mapping
    specularOut += materialOut.r * texture(sampler4, gl_FragCoord.xy).rgb;
    #else //Normal mapping
    specularOut += materialOut.r * texture(sampler4, gl_FragCoord.xy-modelNormal.xy*50.0).rgb;
    #endif //Normal mapping
    #elif REFLECTION_TYPE == 2 //Environment Reflection
    //specularOut += materialOut.r * texture(sampler4, reflect(normalOut, viewVec)).rgb;
    specularOut += texture(sampler4, normalOut).rgb;
    #endif //Environment Reflection
    
    #if REFLECTION_TYPE > 0 || BLENDING_QUALITY > 0
    colorOut.rgb *= min(colorOut.a, 1.0-materialOut.r);
    #endif //Reflection
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