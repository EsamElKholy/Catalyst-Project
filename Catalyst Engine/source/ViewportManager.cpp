#include <ViewportManager.h>

void ViewportManager::AddViewport(const int &x, const int &y, const int &w, const int &h, char *name)
{
	if (name)
	{
		ViewportData *vp = GetViewport(name);

		if (!vp)
		{
			ViewportData v = { x, y, w ,h };

			Viewports[name] = v;
		}
	}
}

ViewportData* ViewportManager::GetViewport(char *name)
{
	if (name)
	{
		std::map<char*, ViewportData>::iterator it = Viewports.find(name);

		if (it != Viewports.end())
		{
			return &(it->second);
		}
	}

	return NULL;
}