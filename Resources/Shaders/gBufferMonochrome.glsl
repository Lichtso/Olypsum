in vec3 position;
out vec3 vPosition;
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform float depthNear, depthFar;
uniform vec4 clipPlane[1];

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
    gl_Position.z = log2(max(gl_Position.w+depthNear, 0.5))*depthFar*gl_Position.w-gl_Position.w;
    vPosition = (vec4(position, 1.0)*modelMat).xyz;
    gl_ClipDistance[0] = dot(vec4(vPosition, 1.0), clipPlane[0]);
}

#separator

uniform vec3 color;
in vec3 vPosition;
out vec3 colorOut;
out vec3 materialOut;
out vec3 normalOut;
out vec3 positionOut;

void main() {
	colorOut = color;
    materialOut = vec3(0.0, 0.0, 0.0);
    normalOut = vec3(0.0);
    positionOut = vPosition;
}