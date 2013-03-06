
#version 330
 
layout(points) in;
layout(triangle_strip, max_vertices=8) out;

in VertexData{
	float shouldDiscard;
	vec3 position;
	int vertexID;
}vertexIn[1];

out vec3 position;
out vec2 texCoord;
out vec3 color;


float random(float c1, float c2)
{
	return fract(sin(c1*(vertexIn[0].vertexID)) * c2);
}

void main()
{
	if(vertexIn[0].shouldDiscard == 0.0)
	{
		float size = (1.0 + random(38.764, 72538.3787) + random(66.529, 2124.48912)) * 1.6 * 1.5;
		float angle = 6.2832 * random(37.064, 77880.3935);

		color.r = 0.8 + 0.3 * random(43.885, 21751.4533);
		color.g = 0.8 + 0.3 * random(50.780, 94699.9964);
		color.b = 0.8 + 0.3 * random(57.329, 49563.7059);

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