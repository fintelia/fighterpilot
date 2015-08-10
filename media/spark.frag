#version 330

uniform sampler2D tex;
uniform sampler2D depthTex;

uniform vec2 invScreenDims;

in vec2 texCoord;
in vec4 color;
in float depth;

out vec4 FragColor;

const float zNear = 1.0;
const float zFar = 2000000.0;

float linearize_depth(float d)
{
    return 2.0 * zNear * zFar / (zFar + zNear - d * (zFar - zNear));
}

void main()
{
    float d = texture(depthTex, invScreenDims * gl_FragCoord.xy).x;
	float bufferDepth = linearize_depth(1.0 - d);

    FragColor = color * texture(tex, texCoord);
    FragColor.a *= clamp(0.5+(bufferDepth-depth),0.0,1.0);
}
