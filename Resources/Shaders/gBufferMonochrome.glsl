in vec3 position;
out vec3 vPosition;
uniform mat4 modelViewMat;
uniform mat4 modelMat;

void main() {
    gl_Position = vec4(position, 1.0)*modelViewMat;
    vPosition = (vec4(position, 1.0)*modelMat).xyz;
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