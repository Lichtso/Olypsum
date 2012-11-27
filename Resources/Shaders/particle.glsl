attribute vec4 position;
uniform mat4 modelViewMat;
uniform float lifeMin;
varying vec3 vPosition;
varying float vAlpha;

void main() {
    gl_Position = vec4(position.xyz, 1.0)*modelViewMat;
    gl_PointSize = clamp(256.0/gl_Position.w, 0.01, 64.0);
    vPosition = position.xyz;
    vAlpha = min(position.w/lifeMin*5.0, 1.0);
}

#separator

uniform sampler2D sampler0;
uniform mat3 viewNormalMat;
varying vec3 vPosition;
varying float vAlpha;

void main() {
    gl_FragData[0] = texture2D(sampler0, gl_PointCoord.xy);
    gl_FragData[0].a *= vAlpha;
    if(gl_FragData[0].a < 0.0039) discard;
    
    vec2 corner = vec2(gl_PointCoord.xy)*2.0-vec2(1.0);
    gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
    gl_FragData[2] = vec4(viewNormalMat[0]*corner.x - viewNormalMat[1]*corner.y + viewNormalMat[2], 1.0);
    gl_FragData[3] = vec4(vPosition + viewNormalMat[0]*corner.x - viewNormalMat[1]*corner.y, 1.0);
}