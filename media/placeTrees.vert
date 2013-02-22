
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

uniform int vertexID_offset;
uniform int patch_width;
uniform int patch_height;
//uniform int x_offset;
//uniform int y_offset;



uniform float placementOdds;

float random(float c1, float c2, int trueVertexID)
{
	return fract(sin(c1*(trueVertexID)) * c2);
}
void main()
{
	int trueVertexID = vertexID_offset + gl_VertexID;//x_offset +int(mod(gl_VertexID,patch_width)) + (gl_VertexID/patch_width + y_offset)*width;

	int patchArea = patch_width * patch_height;
	int a = trueVertexID / patchArea;
	int b = width / patch_width;
	int c = int(mod(trueVertexID, patchArea));

	//vec2 pos = vec2(mod(gl_VertexID, patch_width) + x_offset + random(75.151, 17473.9723, trueVertexID) * 2.0 - 1.0, gl_VertexID/patch_width + y_offset + random(27.091, 25135.1073, trueVertexID) * 2.0 - 1.0);
	vec2 pos = vec2(mod(c, patch_width) + mod(a, b)*patch_width + random(75.151, 17473.9723, trueVertexID) * 2.0 - 1.0, c/patch_width + (a/b)*patch_height + random(27.091, 25135.1073, trueVertexID) * 2.0 - 1.0);
	
	vec4 groundVal = texture2D(groundTex, texOrigin + texSpacing * pos);

	vertexOut.position = worldOrigin + worldSpacing * vec3(pos.x, groundVal.w, pos.y);
	vertexOut.vertexID = trueVertexID;

	vertexOut.shouldDiscard = float(random(75.246,42375.1354, trueVertexID) > placementOdds || vertexOut.position.y <= 40.0 || groundVal.y <= 0.9211 || (vertexOut.position.y >= 150.0 && groundVal.y >= 0.98));
}
