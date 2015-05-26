#version 330

in vec3 position;
in vec2 texCoord;
in float flogz;

uniform sampler2D heightmap;
uniform sampler2D normalmap;

uniform sampler3D oceanNormals;
uniform sampler3D oceanNormals2;

uniform sampler2D grass;

uniform vec3 sunDirection;
uniform vec3 eyePosition;

uniform float time;

out vec4 FragColor;

vec3 readNormal(sampler3D tex, vec2 coord, float time){
    vec3 n = texture(tex, vec3(coord,time)).xzy;
    n.xz = n.xz * 2 - 1;
    return normalize(n);
}

void main()
{
    float height = texture(heightmap, texCoord).x;
    vec3 normal = normalize(texture(normalmap, texCoord).xzy * vec3(2,1,2)
                            - vec3(1,0,1));

    vec3 sNormal = normalize(texture(normalmap, texCoord, 0).xzy * vec3(2,1,2)
                            - vec3(1,0,1));
    
    float slope = length(sNormal.xz) / sNormal.y;
    float nDotL = max(dot(normal,normalize(sunDirection)), 0);// * 0.5 + 0.5;

    vec3 color = vec3(0.2,0.8,0.1);//pow(texture2D(grass, position.xz*0.0005).rgb, vec3(0.7));
//    color = mix(color, vec3(.6, .6, .5), min(slope * 1.0, 1.0));
    color = mix(color, vec3(0.3, 0.3, 0.3), clamp(5*(slope- 0.15), 0.0, 1.0));
//    color = vec3(1.0, 0.0, 0.0);
    color *= nDotL;

    float depth = -(height-1150);
    float waterAmount = clamp(1.0 + depth*10.0, 0, 1);
    vec3 wNormal = readNormal(oceanNormals, position.xz/2000, time/8)
        + readNormal(oceanNormals, position.xz/105, time/4);
    wNormal.y = 1.0;
//        convertNormal();
//        + convertNormal(texture(oceanNormals2, vec3(position.xz/50,time/4)).xyz);
    wNormal = normalize(wNormal);
    
    vec3 wColor;// = vec3(0.0,0.2,0.4) * 2.5;
//    wColor *= dot(wNormal,sunDirection) * 0.5 + 0.5;
    vec3 eyeDir = normalize(eyePosition - position);
    vec3 H = normalize(eyeDir + wNormal);
    float R = 0.02 + (1.0 - 0.02) * (1.0 - pow(dot(H, eyeDir), 5.0));
    vec3 skyColor = vec3(0.1,0.4,0.6);
//    wColor = mix(wColor, skyColor, R);
//    wColor = vec3(R);

    vec3 lightColor = vec3(0.12, 0.56, 0.75);
    vec3 darkColor = /*sunColor */ 0.6 * (exp(-vec3(clamp(depth,0,50)) / vec3(7.0, 30.0, 70.0)));

    wColor = mix(lightColor, darkColor, R)  * dot(wNormal,sunDirection);;


    FragColor = vec4(mix(color, wColor, waterAmount),1);
//    ivec2 v = ivec2(texCoord * 1024);
//    FragColor = vec4(vec3(v.x%2 == v.y%2 ? 0.1 : 0.5), 1);
//    FragColor = vec4(vec3(normal),1);
//    FragColor = vec4(vec3(fract((position.y - 1100) / 300)),1);
	///////////////DEPTH///////////////
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	const float Fcoef = 2.0 / log2(2000000.0 + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	gl_FragDepth = log2(flogz) * Fcoef_half;
}
