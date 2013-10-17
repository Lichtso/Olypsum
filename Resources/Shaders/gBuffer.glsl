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
    vec4 pos = mat*vec4(position, 1.0);
    pos /= pos.w;
    gl_Position = viewMat * pos;
    #if BUMP_MAPPING
    gPosition = pos.xyz;
    gNormal = normalize((mat*vec4(normal, 0.0)).xyz);
    #else
    vPosition = pos.xyz;
    vNormal = normalize((mat*vec4(normal, 0.0)).xyz);
    #endif
    #else
    gl_Position = modelViewMat*vec4(position, 1.0);
    #if BUMP_MAPPING
    gPosition = (modelMat*vec4(position, 1.0)).xyz;
    gNormal = normalMat*normal;
    #else
	vPosition = (modelMat*vec4(position, 1.0)).xyz;
    vNormal = normalMat*normal;
    #endif
    #endif
    #if BUMP_MAPPING
    gTexCoord = texCoord;
    #else
    vTexCoord = texCoord;
    gl_ClipDistance[0] = dot(vec4(vPosition, 1.0), clipPlane[0]);
    #endif
}

#separator

#define M_PI 3.14159265358979323846
//#extension GL_ARB_conservative_depth : enable

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
uniform mat4 camMat;
uniform float depthNear, depthFar;

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

void main() {
    if(vec1Vec2Rand(vTexCoord.st) > discardDensity) discard;
    
    #if BUMP_MAPPING != 2 && BUMP_MAPPING != 3 //No parallax
    setColor(vTexCoord);
    #endif //No parallax
    vec3 viewVec = normalize(camMat[3].xyz-vPosition);
    normalOut = normalize(vNormal);
    positionOut = vPosition;
    //gl_FragDepth = log2(max(1.0/gl_FragCoord.w+depthNear, 0.5))*depthFar*0.5;
    
    #if BUMP_MAPPING > 0
    #if BUMP_MAPPING == 1 //Normal mapping
    vec3 modelNormal = texture(sampler2, vTexCoord).xyz;
    modelNormal.xy = modelNormal.xy*2.0-vec2(1.0);
    #elif BUMP_MAPPING == 4 //Normal noise (water)
    vec3 modelNormal;
    modelNormal.x = vec1Vec3Noise(vec3(vTexCoord.s, vTexCoord.t, texCoordAnimZ.x));
    modelNormal.y = vec1Vec3Noise(vec3(vTexCoord.s+22454.0, vTexCoord.t+6314.0, texCoordAnimZ.x+1373.0));
    modelNormal = normalize(vec3(modelNormal.xy, 1.0));
    #else //Parallax
    #if BUMP_MAPPING == 2 //Parallax simple
    viewVec.xy = vec2(dot(viewVec, vTangent), dot(viewVec, vBitangent));
    vec2 texCoord = vTexCoord-viewVec.xy*texture(sampler2, vTexCoord).a*0.07;
    #elif BUMP_MAPPING == 3 //Parallax occlusion
    vec3 texCoord = vec3(vTexCoord, 0.0), delta = vec3(dot(viewVec, vTangent), dot(viewVec, vBitangent), dot(viewVec, normalOut));
    float depth, factor = 0.1, steps = mix(20.0, 2.0, delta.z);
    delta.xy *= -factor / delta.z;
    delta.z = 1.0;
    delta /= steps;
    while(true) { //Linear Search
        depth = texture(sampler2, texCoord.xy).a;
        if(depth <= texCoord.z) break;
        texCoord += delta;
    }
    for(int i = 0; i < 5; i ++) { //Binary Search
        depth = texture(sampler2, texCoord.xy).a;
		if(depth > texCoord.z)
            texCoord += delta;
		delta *= 0.5;
		texCoord -= delta;
	}
    //positionOut -= viewVec*length(texCoord.xy-vTexCoord)*factor;
    //gl_FragDepth = dot(camMat[3].xyz-positionOut, camMat[2].xyz);
    //gl_FragDepth = log2(max(gl_FragDepth+depthNear, 0.5))*depthFar*0.5;
    #endif //Parallax occlusion
    setColor(texCoord.xy);
    vec3 modelNormal = texture(sampler2, texCoord.xy).xyz;
    modelNormal.xy = modelNormal.xy*2.0-vec2(1.0);
    #endif //Parallax
    normalOut = mat3(vTangent, vBitangent, normalOut) * modelNormal; //Normal
    #endif
    
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
    
    #if REFLECTION_TYPE > 0 //Reflection
    #if REFLECTION_TYPE == 1 //Plane Reflection
    #if BUMP_MAPPING == 0 //No normal mapping
    specularOut += materialOut.g * texture(sampler4, gl_FragCoord.xy).rgb;
    #else //Normal mapping
    specularOut += materialOut.g * texture(sampler4, gl_FragCoord.xy-modelNormal.xy*50.0).rgb;
    #endif //Normal mapping
    #elif REFLECTION_TYPE == 2 //Environment Reflection
    //specularOut += materialOut.g * texture(sampler4, reflect(normalOut, viewVec)).rgb;
    specularOut += materialOut.g * texture(sampler4, normalOut).rgb;
    #endif //Environment Reflection
    colorOut.rgb *= min(colorOut.a, 1.0-materialOut.g);
    #elif BLENDING_QUALITY > 0 //Blending
    colorOut.rgb *= colorOut.a;
    #endif //Blending
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