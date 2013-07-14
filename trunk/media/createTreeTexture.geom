
#version 330
 
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in VertexData{
	float shouldDiscard;
	vec3 position;
	int vertexID;
}vertexIn[1];
 
out VertexData{
	vec2 texCoord;
	vec3 color;
}vertexOut;

uniform vec2 transform;


float random(float c1, float c2)
{
	return fract(cos(sin(c1*(vertexIn[0].vertexID)) * c2));
}

void main()
{
	if(vertexIn[0].shouldDiscard == 0.0)
	{
		float size = (1.0 + random(38.764, 72538.3787) + random(66.529, 2124.48912)) * 1.6 * 1.5;

		vertexOut.color.r = 0.8 + 0.3 * random(43.885, 21751.4533);
		vertexOut.color.g = 0.8 + 0.3 * random(50.780, 94699.9964);
		vertexOut.color.b = 0.8 + 0.3 * random(57.329, 49563.7059);

		gl_Position = vec4((vertexIn[0].position.xz + vec2(-2.5,-2.5) * size) * transform / 2048 - 1.0, 0.0, 1.0); 
		vertexOut.texCoord = vec2(0,0);
		EmitVertex();

		gl_Position = vec4((vertexIn[0].position.xz + vec2(-2.5,2.5) * size) * transform / 2048 - 1.0, 0.0, 1.0);
		vertexOut.texCoord = vec2(0,1);
		EmitVertex();
		
		gl_Position = vec4((vertexIn[0].position.xz + vec2(2.5,-2.5) * size) * transform / 2048 - 1.0, 0.0, 1.0);
		vertexOut.texCoord = vec2(1,0);
		EmitVertex();

		gl_Position = vec4((vertexIn[0].position.xz + vec2(2.5,2.5) * size) * transform / 2048 - 1.0, 0.0, 1.0);
		vertexOut.texCoord = vec2(1,1);
		EmitVertex();

		EndPrimitive();
	}
}