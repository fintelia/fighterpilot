#version 330

in vec2 Position2;

out VertexData{
	float shouldDiscard;
	vec3 position;
	uint vertexID;
}vertexOut;

uniform sampler2D heights;

uniform vec2 texOrigin;
uniform vec2 texSpacing;

uniform vec3 worldOrigin;
uniform vec3 worldSpacing;
uniform int width;

uniform int vertexID_offset;
uniform int patch_width;
uniform int patch_height;

uniform float earthRadius;
uniform float slopeScale;

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
float random(float c1, float c2, uint trueVertexID)
{
    uint h = hash(trueVertexID + uint(float(trueVertexID) * c1 + c2));
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    return uintBitsToFloat(h) - 1.0;
}

void main()
{
	uint trueVertexID = uint(vertexID_offset + gl_InstanceID);

	int patchArea = patch_width * patch_height;
	int a = int(trueVertexID) / patchArea;
	int b = width / patch_width;
	int c = int(mod(int(trueVertexID), patchArea));

	vec2 pos = vec2(mod(c, patch_width) + mod(a, b) * patch_width +
	                    random(75.151, 17473.9723, trueVertexID) * 2.0 - 1.0,
	                c / patch_width + (a / b) * patch_height +
	                    random(27.091, 25135.1073, trueVertexID) * 2.0 - 1.0);

	vec2 texCoord = texOrigin + texSpacing * pos;
	float height = texture(heights, texCoord).r;
	// vec3 normal = normalize(texture(normals, texCoord).xzy * vec3(2, 1, 2) -
	//                         vec3(1, 0, 1));
	// float slope = length(normal.xy) / normal.y;

	vertexOut.position =
	    worldOrigin + worldSpacing * vec3(pos.x, height, pos.y);
	vertexOut.vertexID = trueVertexID;

	vec2 r = vertexOut.position.xz / earthRadius;
	//vertexOut.position.y += earthRadius * (sqrt(1.0 - dot(r,r)) - 1.0);
	

	float odds = clamp(0.05 * (vertexOut.position.y - 10.0 - 1150), 0.0, 1.0);
		/*	             clamp(20 * (0.9 - slope), 0.0, 1.0) *
						 clamp(5 * (slope - 0.05), 0.0, 1.0);*/
	
	vertexOut.shouldDiscard = float(random(75.246,42375.1354, trueVertexID) > odds /*|| groundVal.y <= 0.9111 ||(vertexOut.position.y >= 150.0 && groundVal.y >= 0.98)*/);
}
