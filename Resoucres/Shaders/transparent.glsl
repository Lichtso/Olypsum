attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
#if SKELETAL_ANIMATION
attribute vec3 weights;
attribute vec3 joints;

uniform mat4 jointMats[64];
uniform mat4 viewMat;
#else
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
#endif

#if BUMP_MAPPING > 0
varying vec3 gPosition;
varying vec2 gTexCoord;
varying vec3 gNormal;
#else
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
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
    #endif
    #endif
}

#separator

#define M_PI 3.14159265358979323846

varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
#if BUMP_MAPPING
varying vec3 vTangent;
varying vec3 vBitangent;
#endif

uniform mat3 viewNormalMat;
uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2DRect sampler3;
uniform float discardDensity;
#if BUMP_MAPPING == 2
const int maxWaves = 4;
uniform float waveLen[maxWaves];
uniform float waveAge[maxWaves];
uniform float waveAmp[maxWaves];
uniform vec2 waveOri[maxWaves];
#endif

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    gl_FragData[0] = texture2D(sampler0, vTexCoord); //Color
    if(gl_FragData[0].a < 0.0039 || random(gl_FragCoord.xy) > discardDensity) discard;
    gl_FragData[1] = vec4(texture2D(sampler1, vTexCoord).rgb, 1.0); //Material
    gl_FragData[1] = vec4(0.8, 0.8, 0.0, 1.0); //Material
    gl_FragData[0].a = 0.5;
    
    vec3 normal = normalize(vNormal);
    #if BUMP_MAPPING == 1
    vec3 bumpMap = texture2D(sampler2, vTexCoord).xyz;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    normal = mat3(vTangent, vBitangent, normal)*bumpMap;
    #elif BUMP_MAPPING == 2
    sampler2;
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
    normal = mat3(vTangent, vBitangent, normal)*normalize(bumpMap);
    #endif
    
    #if BLENDING_QUALITY == 2
    gl_FragData[0].rgb *= gl_FragData[0].a;
    gl_FragData[0].rgb += (1.0-gl_FragData[0].a)*texture2DRect(sampler3, gl_FragCoord.xy+(viewNormalMat*normal).xy*10.0).rgb; //Color
    gl_FragData[0].a = 1.0;
    #elif BLENDING_QUALITY == 3
    gl_FragData[4].rgb = (1.0-gl_FragData[0].a)*texture2DRect(sampler3, gl_FragCoord.xy+(viewNormalMat*normal).xy*10.0).rgb; //Specular
    gl_FragData[4].a = 1.0;
    #endif
    
    gl_FragData[2] = vec4(normal, 1.0); //Normal
	gl_FragData[3] = vec4(vPosition, 1.0); //Position
}

#separator

#parameter GL_GEOMETRY_VERTICES_OUT 3
#parameter GL_GEOMETRY_INPUT_TYPE GL_TRIANGLES
#parameter GL_GEOMETRY_OUTPUT_TYPE GL_TRIANGLE_STRIP
#extension GL_EXT_geometry_shader4: enable

varying in vec3 gPosition[3];
varying in vec2 gTexCoord[3];
varying in vec3 gNormal[3];
varying out vec3 vPosition;
varying out vec2 vTexCoord;
varying out vec3 vNormal;
varying out vec3 vTangent;
varying out vec3 vBitangent;

void main() {
	vec3 posBA = gPosition[1]-gPosition[0], posCA = gPosition[2]-gPosition[0];
	vec2 texBA = gTexCoord[1]-gTexCoord[0], texCA = gTexCoord[2]-gTexCoord[0];
	vec3 tangent = normalize((texCA.t*posBA-texBA.t*posCA) / (texBA.s*texCA.t-texBA.t*texCA.s)),
		 bitangent = normalize((texCA.s*posBA-texBA.s*posCA) / (texBA.t*texCA.s-texBA.s*texCA.t));
    
    for(int i = 0; i < 3; i ++) {
		gl_Position = gl_PositionIn[i];
		vPosition = gPosition[i];
		vTexCoord = gTexCoord[i];
		vNormal = gNormal[i];
		vTangent = tangent;
		vBitangent = bitangent;
		EmitVertex();
	}
}