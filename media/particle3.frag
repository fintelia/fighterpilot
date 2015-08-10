#version 330

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D depth;

uniform vec2 invScreenDims;

in VertexData{
	vec4 color;
	vec2 texCoord;
	float invRadius;
	float depth;
}vertexIn;

const float zNear = 1.0;
const float zFar = 2000000.0;

float linearize_depth(float d)
{
    return 2.0 * zNear * zFar / (zFar + zNear - d * (zFar - zNear));
}

void main()
{
    float d = texture(depth, invScreenDims * gl_FragCoord.xy).x;
	float bufferDepth = linearize_depth(1.0 - d);

    FragColor = vertexIn.color * texture(tex, vertexIn.texCoord);
    FragColor.a *= clamp(0.5+(bufferDepth-vertexIn.depth) *
                        vertexIn.invRadius,0.0,1.0);
}
