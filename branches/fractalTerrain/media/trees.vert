
varying vec2 texCoord;
varying vec3 position;
varying vec3 treeColor;
varying float flogz;

attribute vec3 Position;
attribute vec2 TexCoord;
attribute vec3 Color3;

uniform mat4 cameraProjection;

void main()
{
	//vec3 right = normalize(cross(vec3(0,1,0),Position-eyePos));
	texCoord = TexCoord;
	position = Position;// + right * (TexCoord.x*4.0-1.0) * scale;
	treeColor = Color3;

	gl_Position = cameraProjection * vec4(position,1.0);
	
	flogz = 1.0 + gl_Position.w;
}
