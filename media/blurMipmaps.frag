

varying vec2 position;
uniform sampler2D tex;
uniform vec2 invTexSize;
uniform int readLod;




void main()
{
	//vec4 color = vec4(0,0,0,0);
	//
	//
	//for(int x=-1; x<=1; x++)
	//{
	//	for(int y=-1; y<=1; y++)
	//	{
	//		color += texture2DLod(tex, position + vec2( x, y)*invTexSize,readLod);
	//	}
	//}
	//gl_FragColor = color / 9.0;

	gl_FragColor = (texture2DLod(tex, position + vec2( 0.5, 0.5)*invTexSize,readLod) + 
					texture2DLod(tex, position + vec2(-0.5, 0.5)*invTexSize,readLod) +
					texture2DLod(tex, position + vec2( 0.5,-0.5)*invTexSize,readLod) +
					texture2DLod(tex, position + vec2(-0.5,-0.5)*invTexSize,readLod)) * 0.25;
}