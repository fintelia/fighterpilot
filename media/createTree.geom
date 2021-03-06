
#version 330
 
layout(points) in;
layout(triangle_strip, max_vertices=8) out;

in VertexData{
	float shouldDiscard;
	vec3 position;
	uint vertexID;
}vertexIn[1];

out vec3 position;
out vec2 texCoord;
out vec3 color;


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
		float angle = 6.2832 * random(37.064, 77880.3935);

		color.r = 0.7 + 0.5 * random(43.885, 21751.4533);
		color.g = 0.7 + 0.5 * random(50.780, 94699.9964);
		color.b = 0.7 + 0.5 * random(57.329, 49563.7059);

		vec2 dir = vec2(cos(angle), sin(angle)) * 2.5;



		//TRIANGLE STRIP 1
		position = vertexIn[0].position + vec3(-dir.x,0,-dir.y) * size; 
		texCoord = vec2(0,1);
		EmitVertex();

		position = vertexIn[0].position + vec3(dir.x,0,dir.y) * size;
		texCoord = vec2(0.5,1);
		EmitVertex();
		
		position = vertexIn[0].position + vec3(-dir.x,5,-dir.y) * size;
		texCoord = vec2(0,0);
		EmitVertex();

		position = vertexIn[0].position + vec3(dir.x,5,dir.y) * size;
		texCoord = vec2(0.5,0);
		EmitVertex();

		EndPrimitive();
		dir = vec2(dir.y, -dir.x);



		//TRIANGLE STRIP 2	
		position = vertexIn[0].position + vec3(-dir.x,0,-dir.y) * size;
		texCoord = vec2(0.5,1);
		EmitVertex();
		
		position = vertexIn[0].position + vec3(dir.x,0,dir.y) * size;
		texCoord = vec2(1,1);
		EmitVertex();
		
		position = vertexIn[0].position + vec3(-dir.x,5,-dir.y) * size;
		texCoord = vec2(0.5,0);
		EmitVertex();

		position = vertexIn[0].position + vec3(dir.x,5,dir.y) * size;
		texCoord = vec2(1,0);
		EmitVertex();

		EndPrimitive();

	}
}