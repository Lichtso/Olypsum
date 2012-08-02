attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;

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
}

#separator

varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
#if BUMP_MAPPING > 0
varying vec3 vTangent;
varying vec3 vBitangent;

uniform vec3 camPos;
uniform mat3 viewNormalMat;
uniform sampler2D sampler2;
uniform sampler2DRect sampler3;
#endif
uniform sampler2D sampler0;
uniform sampler2D sampler1;
#if BUMP_MAPPING < 2
uniform float discardDensity;
#else
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
    #if BUMP_MAPPING < 2
    gl_FragData[0] = texture2D(sampler0, vTexCoord); //Color
    if(gl_FragData[0].a < 0.0039 || random(gl_FragCoord.xy) > discardDensity) discard;
    gl_FragData[1] = vec4(texture2D(sampler1, vTexCoord).rgb, 1.0); //Material
    #else
    gl_FragData[1] = vec4(0.2, 0.8, 0.0, 1.0); //Material
    #endif
    
    vec3 normal = normalize(vNormal);
    #if BUMP_MAPPING == 1
    vec3 bumpMap = texture2D(sampler2, vTexCoord).xyz;
    bumpMap.xy = bumpMap.xy*2.0-vec2(1.0);
    normal = mat3(vTangent, vBitangent, normal)*bumpMap;
    #elif BUMP_MAPPING == 2
    vec3 bumpMap = vec3(0.0); //TODO water normals
    normal = mat3(vTangent, vBitangent, normal)*bumpMap;
    #endif
	
	vec3 light = vec3(1.0);
	//TODO: Illumination
	gl_FragData[0].rgb *= light;
    
    #if BUMP_MAPPING
	vec2 displace = (viewNormalMat*normal).xy;
    #endif
    
    #if BUMP_MAPPING == 2
	gl_FragData[0] = texture2D(sampler0, vTexCoord+displace*0.01); //Color
    #endif
    
    #if BUMP_MAPPING
	gl_FragData[0].rgb *= gl_FragData[0].a;
	gl_FragData[0].rgb += (1.0-gl_FragData[0].a)*texture2DRect(sampler3, gl_FragCoord.xy+displace*10.0).rgb; //TODO: bumpMap to Refraction
	gl_FragData[0].a = 1.0;
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