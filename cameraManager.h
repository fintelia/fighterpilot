
class CameraManager
{
private:
	struct view
	{
		Vec3f	eye,
				center,
				up;
		Angle	fovy;
		float	zNear,
				zFar;
		Vec2f	origin,
				size;
		view();
	};
	vector<view> views;
	unsigned int currentView;
	CameraManager();
public:
	static CameraManager& getInstance()
	{
		static CameraManager* pInstance = new CameraManager();
		return *pInstance;
	}

	view& operator() (int v);
	view& operator() ();
	unsigned int numViews();
	void setCurrentView(unsigned int nView);
};

extern CameraManager cameraManager;