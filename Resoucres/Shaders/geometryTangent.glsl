#parameter GL_GEOMETRY_VERTICES_OUT 3
#parameter GL_GEOMETRY_INPUT_TYPE GL_TRIANGLES
#parameter GL_GEOMETRY_OUTPUT_TYPE GL_TRIANGLE_STRIP
#extension GL_EXT_geometry_shader4: enable

varying in vec3 gPosition[3];
varying in vec2 gTexcoord[3];
varying in vec3 gNormal[3];
varying out vec3 vPosition;
varying out vec2 vTexcoord;
varying out vec3 vNormal;
varying out vec3 vTangent;
varying out vec3 vBitangent;

void main() {
	vec3 posBA = gPosition[1]-gPosition[0], posCA = gPosition[2]-gPosition[0];
	vec2 texBA = gTexcoord[1]-gTexcoord[0], texCA = gTexcoord[2]-gTexcoord[0];
	vec3 tangent = (texCA.t*posBA-texBA.t*posCA) / (texBA.s*texCA.t-texBA.t*texCA.s),
		 bitangent = (texCA.s*posBA-texBA.s*posCA) / (texBA.t*texCA.s-texBA.s*texCA.t);
	
    for(int i = 0; i < 3; i ++) {
		gl_Position = gl_PositionIn[i];
		vPosition = gPosition[i];
		vTexcoord = gTexcoord[i];
		vNormal = gNormal[i];
		vTangent = tangent;
		vBitangent = bitangent;
		EmitVertex();
	}
 }