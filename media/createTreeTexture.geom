
#version 330
 
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in VertexData{
	float shouldDiscard;
	vec3 position;
	uint vertexID;
}vertexIn[1];
 
out VertexData{
	vec2 texCoord;
	vec3 color;
}vertexOut;

uniform float scale;

//see:http://amindforeverprogramming.blogspot.com/2013/07/random-floats-in-glsl-330.html
uint hash(uint x)
{
    x += x << 10u;
    x ^= x >>  6u;
    x += x <<  3u;
    x ^= x >> 11u;
    x += x << 15u;
    return x;
}
float random(float c1, float c2)
{
    uint h = hash(vertexIn[0].vertexID + uint(float(vertexIn[0].vertexID) * c1 + c2));
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    
    return uintBitsToFloat(h) - 1.0;
}

void main()
{
	if(vertexIn[0].shouldDiscard == 0.0)
	{
		float size = (1.0 + random(38.764, 72538.3787) + random(66.529, 2124.48912)) * 1.6 * 1.5;

		vertexOut.color.r = 0.8 + 0.3 * random(43.885, 21751.4533);
		vertexOut.color.g = 0.8 + 0.3 * random(50.780, 94699.9964);
		vertexOut.color.b = 0.8 + 0.3 * random(57.329, 49563.7059);

		gl_Position = vec4(scale * (vertexIn[0].position.xz + vec2(-2.5,-2.5) * size) - 1.0, 0.0, 1.0); 
		vertexOut.texCoord = vec2(0,0);
		EmitVertex();

		gl_Position = vec4(scale * (vertexIn[0].position.xz + vec2(-2.5,2.5) * size) - 1.0, 0.0, 1.0);
		vertexOut.texCoord = vec2(0,1);
		EmitVertex();
		
		gl_Position = vec4(scale * (vertexIn[0].position.xz + vec2(2.5,-2.5) * size) - 1.0, 0.0, 1.0);
		vertexOut.texCoord = vec2(1,0);
		EmitVertex();

		gl_Position = vec4(scale * (vertexIn[0].position.xz + vec2(2.5,2.5) * size) - 1.0, 0.0, 1.0);
		vertexOut.texCoord = vec2(1,1);
		EmitVertex();

		EndPrimitive();
	}
}