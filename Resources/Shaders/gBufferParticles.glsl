in vec4 position;
in vec4 velocity;
uniform float textureZScale;
uniform float textureAScale;
out vec3 gPosition;
out vec2 gLife;
out float gSize;

void main() {
    gPosition = position.xyz;
    gLife.x = position.w*textureZScale;
    gLife.y = min(position.w*textureAScale, 1.0);
    gSize = velocity.w;
}

#separator

in vec3 vPosition;
in vec4 vTexCoord;
in vec3 vNormal;
out vec4 colorOut;
out vec4 materialOut;
out vec4 normalOut;
out vec4 positionOut;
uniform float depthNear, depthFar;
#if TEXTURE_ANIMATION == 0
uniform sampler2D sampler0;
#else
uniform sampler2DArray sampler0;
#endif

void main() {
    #if TEXTURE_ANIMATION == 0 //2D texture
    colorOut = texture(sampler0, vTexCoord.xy); //Color
    #else //3D texture
    float interpolZ = fract(vTexCoord.z);
    colorOut = texture(sampler0, vec3(vTexCoord.xy, vTexCoord.z-0.5))*(1.0-interpolZ); //Color
    colorOut += texture(sampler0, vec3(vTexCoord.xy, vTexCoord.z+0.5))*interpolZ;
    #endif
    colorOut.a *= vTexCoord.w;
    if(colorOut.a < 0.0039) discard;
    
    materialOut = vec4(0.0, 0.0, 0.0, 1.0);
    normalOut = vec4(vNormal, colorOut.a);
    positionOut = vec4(vPosition, colorOut.a);
    gl_FragDepth = log2(max(1.0/gl_FragCoord.w+depthNear, 0.5))*depthFar*0.5; //Depth
}

#separator

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 viewMat;
uniform mat4 camMat;
uniform vec4 clipPlane[1];
in vec3 gPosition[1];
in vec2 gLife[1];
in float gSize[1];
out vec3 vPosition;
out vec4 vTexCoord;
out vec3 vNormal;
out float gl_ClipDistance[1];

void main() {
    float size = gSize[0];
    vec3 rightVec = camMat[0].xyz*size;
    vec3 upVec = camMat[1].xyz*size;
    vNormal = camMat[2].xyz-camMat[0].xyz+camMat[1].xyz;
    
    vPosition = gPosition[0]-rightVec+upVec;
    gl_Position = viewMat*vec4(vPosition.xyz, 1.0);
	vTexCoord = vec4(0.0, 0.0, gLife[0]);
    gl_ClipDistance[0] = dot(vec4(vPosition.xyz, 1.0), clipPlane[0]);
	EmitVertex();
    
    vPosition = gPosition[0]-rightVec-upVec;
    gl_Position = viewMat*vec4(vPosition.xyz, 1.0);
	vTexCoord = vec4(0.0, 1.0, gLife[0]);
    gl_ClipDistance[0] = dot(vec4(vPosition.xyz, 1.0), clipPlane[0]);
	EmitVertex();
    
    vPosition = gPosition[0]+rightVec+upVec;
    gl_Position = viewMat*vec4(vPosition.xyz, 1.0);
	vTexCoord = vec4(1.0, 0.0, gLife[0]);
    gl_ClipDistance[0] = dot(vec4(vPosition.xyz, 1.0), clipPlane[0]);
	EmitVertex();
    
    vPosition = gPosition[0]+rightVec-upVec;
    gl_Position = viewMat*vec4(vPosition.xyz, 1.0);
	vTexCoord = vec4(1.0, 1.0, gLife[0]);
    gl_ClipDistance[0] = dot(vec4(vPosition.xyz, 1.0), clipPlane[0]);
	EmitVertex();
}