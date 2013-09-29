in vec3 position;
out vec3 vPosition;
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform vec4 clipPlane[1];

void main() {
    gl_Position = modelViewMat*vec4(position, 1.0);
    vPosition = (modelMat*vec4(position, 1.0)).xyz;
    gl_ClipDistance[0] = dot(vec4(vPosition, 1.0), clipPlane[0]);
}

#separator

in vec3 vPosition;
out vec3 colorOut;
out vec3 materialOut;
out vec3 normalOut;
out vec3 positionOut;
uniform vec3 color;
uniform float depthNear, depthFar;

void main() {
	colorOut = color;
    materialOut = vec3(0.0, 0.0, 0.0);
    normalOut = vec3(0.0);
    positionOut = vPosition;
    gl_FragDepth = log2(max(1.0/gl_FragCoord.w+depthNear, 0.5))*depthFar*0.5;
}