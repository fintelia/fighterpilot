
#version 330

//out vec3 position;
//out vec2 texCoord;

//in vec3 Position;
//in vec2 TexCoord;

out VertexData{
	float shouldDiscard;
	vec3 position;
	int vertexID;
}vertexOut;

uniform sampler2D groundTex;

uniform vec2 texOrigin;
uniform vec2 texSpacing;

uniform vec3 worldOrigin;
uniform vec3 worldSpacing;
uniform int width;

uniform float placementOdds;

float random(float c1, float c2)
{
	return fract(sin(c1*(gl_VertexID)) * c2);
}

void main()
{
	vec2 pos = vec2(mod(gl_VertexID, width) + random(75.151, 17473.9723) * 2.0 - 1.0, gl_VertexID/width + random(27.091, 25135.1073) * 2.0 - 1.0);
	vec4 groundVal = texture2D(groundTex, texOrigin + texSpacing * pos);

	vertexOut.position = worldOrigin + worldSpacing * vec3(pos.x, groundVal.w, pos.y);
	vertexOut.vertexID = gl_VertexID;

	//vertexOut.shouldDiscard =	mod(gl_VertexID, 5) +														// gl_VertexID % 5 != 0 ||
	//							min(0.0, vertexOut.position.y - 40.0) +										// vertexOut.position.y >= 40.0 ||
	//							min(0.0, groundVal.y - 0.9211) +											// groundVal.y <= 0.9211 ||
	//							(min(0.0, vertexOut.position.y - 150.0) * min(0.0, 0.98 - groundVal.y));	// vertexOut.position.y >  && 
	//							; //discards if modulo != 0 or position.y > 40
	//
	vertexOut.shouldDiscard = float(random(75.246,42375.1354) > placementOdds || vertexOut.position.y <= 40.0 || groundVal.y <= 0.9211 || (vertexOut.position.y >= 150.0 && groundVal.y >= 0.98));
}
