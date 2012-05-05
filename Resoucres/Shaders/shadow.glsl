attribute vec3 position;
attribute vec2 texCoord;
uniform mat4 modelViewMat;
uniform mat4 viewMat;
uniform float paraboloidRange;
varying vec2 vTexCoord;
varying vec3 vPosition;

void main() {
	gl_Position = vec4(position, 1.0)*modelViewMat;
	vTexCoord = texCoord;
    if(paraboloidRange == 0.0) {
        vPosition = vec3(gl_Position.z*0.5, gl_Position.w, 0.5);
    }else{
        float len = length(gl_Position.xyz);
        gl_Position.z *= -1.0;
        vPosition.z = gl_Position.z+0.05;
        gl_Position.xy /= gl_Position.z + len;
        gl_Position.z = len / paraboloidRange;
        vPosition.xy = vec2(gl_Position.z-0.5, 1.0);
        gl_Position.w = 1.0;
    }
}

#|#|#

uniform sampler2D sampler0;
varying vec2 vTexCoord;
varying vec3 vPosition;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	if(gl_FragColor.a < 0.1 || vPosition.z < 0.0) discard;
    float depth = vPosition.x/vPosition.y+0.5;
    gl_FragColor.r = floor(depth*255.0)/255.0;
    gl_FragColor.g = floor((depth-gl_FragColor.r)*65535.0)/255.0;
    gl_FragColor.b = floor((depth-gl_FragColor.g)*16777215.0)/255.0;
    gl_FragColor.a = floor((depth-gl_FragColor.a)*4294967296.0)/255.0;
}