#version 330

uniform float middleGrey = 0.36;
uniform float L_white = 1000.0;

uniform sampler2D tex;
uniform sampler2D logLuminance;
// uniform sampler2D bloomTex;

in vec2 texCoord;
out vec4 FragColor;


void main()
{
    float avgLuminance = 0.25;//exp(texture(logLuminance, texCoord, 32).r);
    float exposure = middleGrey / avgLuminance;

    vec3 Lw = texture(tex, texCoord).rgb;// +  
	//              max(textureLod(bloomTex, texCoord, 3).rgb * 0.5, 0);
    vec3 L = exposure * Lw;
    vec3 outputColor = L * (1.0 + L/(L_white*L_white)) / (1 + L);

    float outputLuminance = dot(outputColor, vec3(0.2126, 0.7152, 0.0722));    
	FragColor = vec4(outputColor, outputLuminance);
}
