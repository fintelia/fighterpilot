
uniform mat4 cameraProjection;
uniform mat4 modelTransform;
attribute vec3 Position;

void main()
{
	gl_Position = cameraProjection * modelTransform * vec4(Position,1.0);
}
