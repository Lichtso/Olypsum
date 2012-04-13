attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
uniform mat4 modelViewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
uniform mat4 shadowMat;
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec4 sTexCoord;
varying vec3 vNormal;

void main() {
	gl_Position = vec4(position, 1.0)*modelViewMat;
	vPosition = (vec4(position, 1.0)*modelMat).xyz;
	vNormal = normal*normalMat;
	vTexCoord = texCoord;
	sTexCoord = vec4(position, 1.0)*shadowMat;
}

#|#|#

varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec4 sTexCoord;
varying vec3 vNormal;
uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform int shadows;
uniform vec3 sunVec;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord);
	if(gl_FragColor.a < 0.1) discard;
	
	vec3 light = texture2D(sampler1, sTexCoord.st/sTexCoord.q).rgb;
	float depthRef = light.r+light.g/256.0+light.b/65536.0;
	light = vec3(max(0.15, vNormal.z));
	
	if(shadows > 0)
		if(depthRef == 0.0 || depthRef*0.9989 < -sTexCoord.z) {
			light = vec3(max(0.2, min(1.0, dot(sunVec, vNormal)*2.0)));
		}else{
			light = vec3(0.2);
		}
	
    vPosition;
    
	gl_FragColor.rgb *= light.rgb;
}