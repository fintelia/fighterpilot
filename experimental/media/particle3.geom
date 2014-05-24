#version 330
 
layout(points) in;
layout(triangle_strip, max_vertices=4) out; // triangle_strip
 
in VertexData{
	vec4 color;
	float radius;
	float angle;
}vertexIn[1];
 
out VertexData{
	vec4 color;
	vec2 texCoord;
	float invRadius;
}vertexOut;

uniform vec3 up;
uniform vec3 right;
uniform mat4 cameraProjection;

void main()
{
	vertexOut.color = vertexIn[0].color;
	vertexOut.invRadius = 1.0/vertexIn[0].radius;
	
	float sAng = sin(vertexIn[0].angle);
	float cAng = cos(vertexIn[0].angle);
	
	vec4 U = vec4((-right * sAng + up * cAng) * vertexIn[0].radius, 0.0);
	vec4 R = vec4(( right * cAng + up * sAng) * vertexIn[0].radius, 0.0);
	
	gl_Position = cameraProjection * (gl_in[0].gl_Position + U + R);
	vertexOut.texCoord  = vec2(0,0);
	EmitVertex();
	
	gl_Position = cameraProjection * (gl_in[0].gl_Position + U - R);
	vertexOut.texCoord  = vec2(0,1);
	EmitVertex();

	gl_Position = cameraProjection * (gl_in[0].gl_Position - U + R);
	vertexOut.texCoord = vec2(1,0);
	EmitVertex();

	gl_Position = cameraProjection * (gl_in[0].gl_Position - U - R);
	vertexOut.texCoord  = vec2(1,1);
	EmitVertex();

	//VertexOut.color = vec4(0,0,1,1);//VertexIn.color;
	//VertexOut.radius  = VertexIn[0].radius;
	//VertexOut.texCoord  = vec2(1,0);
	//
	//
	
	//color = vec4(0,1,0,1);//vertexIn[0].color;
	//radius  = vertexIn[0].radius;
	//

	//texCoord  = vec2(1,0);
	//gl_Position = cameraProjection * gl_in[0].gl_Position + vec4(0.0, 0.0,  0, 0);	EmitVertex();
	//gl_Position = cameraProjection * gl_in[0].gl_Position + vec4(1.0, 0.0,  0, 0);	EmitVertex();
	//gl_Position = cameraProjection * gl_in[0].gl_Position + vec4(0.0, 1.0, 0, 0);	EmitVertex();
	//gl_Position = cameraProjection * gl_in[0].gl_Position + vec4(1.0, 1.0, 0, 0);	EmitVertex();
}