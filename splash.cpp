//
//namespace particle
//{
//	splash::splash(Vec3f pos)
//	{
//		parentObject = parent;
//		parentOffset = offset;
//		position = world.objectList[parentObject]->position + world.objectList[parentObject]->rotation * parentOffset;
//		init();
//		velocity = fuzzyAttribute(3.0, 1.0);
//		spread = fuzzyAttribute(1.0, 0.5);
//		life = fuzzyAttribute(6000.0,100.0);
//
//		friction = 1.0;
//		particlesPerSecond = 3.0;
//
//		glGenBuffers(1,&VBO);
//	}
//	splash::splash(int parent, Vec3f offset = Vec3f(0,0,0))
//	{
//		position = pos;
//		init();
//	}
//	void splash::init()
//	{
//
//	}
//	void splash::update()
//	{
//
//	}
//	void splash::render(Vec3f up, Vec3f right)
//	{
//
//	}
//}