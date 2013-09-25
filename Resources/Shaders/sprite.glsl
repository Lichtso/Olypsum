in vec2 position;

out vec2 vTexCoord;
uniform mat4 modelViewMat;
uniform mat3 textureMat;

void main() {
    gl_Position = modelViewMat*vec4(position, 0.0, 1.0);
    vTexCoord = (textureMat*vec3(position, 1.0)).xy;
}

#separator

in vec2 vTexCoord;
out vec4 colorOut;
uniform float alpha;
uniform sampler2D sampler0;

void main() {
	colorOut = texture(sampler0, vTexCoord.st);
    colorOut.a *= alpha;
    if(colorOut.a < 0.0039) discard;
}