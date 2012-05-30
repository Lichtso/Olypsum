attribute vec3 position;
attribute vec3 texCoord;
attribute vec2 tangent;

#define lightsCount 3
struct lightSource {
    float type, range, cutoff, shadowFactor;
	vec3 color, position, direction;
    mat4 shadowMat;
};

uniform lightSource lightSources[lightsCount];
uniform mat4 modelViewMat;
uniform mat4 viewMat;
uniform vec3 color;
varying vec3 vTexCoord;
varying vec3 lightColor;

void main() {
    vec3 pos = position + normalize(viewMat[0].xyz)*tangent.x + normalize(viewMat[1].xyz)*tangent.y;
	gl_Position = vec4(pos, 1.0)*modelViewMat;
	vTexCoord = texCoord;
    
    vec3 lightDir;
    float lightIntensity;
    if(color.r == -1.0) {
        lightColor = vec3(0.2);
        for(int i = 0; i < lightsCount; i ++) {
            if(lightSources[i].type == 0.0) continue;
            if(lightSources[i].type == 1.0)
                lightDir = lightSources[i].direction;
            else
                lightDir = lightSources[i].position-pos.xyz;
        
            lightIntensity = length(lightDir);
            lightDir /= lightIntensity;
            if(lightSources[i].type == 2.0 && acos(-dot(lightSources[i].direction, lightDir)) > lightSources[i].cutoff)
                continue;
            lightColor += lightSources[i].color * (1.0 - lightIntensity / lightSources[i].range);
        }
        lightColor = min(lightColor, vec3(1.0));
    }else{
        lightColor = vec3(1.0);
    }
}

#|#|#

uniform sampler2D sampler0;
uniform vec3 color;
varying vec3 vTexCoord;
varying vec3 lightColor;

void main() {
	gl_FragColor = texture2D(sampler0, vTexCoord.st);
    gl_FragColor.a *= 1.0 - vTexCoord.z;
    if(gl_FragColor.a < 0.1) discard;
    if(color.r > -1.0) gl_FragColor.rgb += color;
    gl_FragColor.rgb *= lightColor;
}