in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}

#separator

out vec4 normalOut;
uniform sampler2DRect sampler0;
uniform float processingValue;

void main() {
    normalOut.a = texture(sampler0, gl_FragCoord.xy).r;
    vec3 pos = vec3(0.0), posL = vec3(-1.0, 0.0, 0.0), posR = vec3(1.0, 0.0, 0.0),
         posB = vec3(0.0, -1.0, 0.0), posT = vec3(0.0, 1.0, 0.0);
    pos.z = normalOut.a*processingValue;
    posL.z = texture(sampler0, gl_FragCoord.xy+posL.xy).r*processingValue;
    posR.z = texture(sampler0, gl_FragCoord.xy+posR.xy).r*processingValue;
    posB.z = texture(sampler0, gl_FragCoord.xy+posB.xy).r*processingValue;
    posT.z = texture(sampler0, gl_FragCoord.xy+posT.xy).r*processingValue;
    posL -= pos; posR -= pos; posB -= pos; posT -= pos;
    vec3 normal = normalize(cross(posT, posL)+cross(posR, posT)+cross(posL, posB)+cross(posB, posR));
    normalOut.rgb = vec3(vec2(0.5)-normal.xy*0.5, normal.z);
}