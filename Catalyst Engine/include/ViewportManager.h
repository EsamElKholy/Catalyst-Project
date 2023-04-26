#pragma once

#include <GL\glew.h>
#include <map>

struct ViewportData
{
	int X;
	int Y;
	int Width;
	int Height;
};

class ViewportManager
{
public:
	ViewportManager() {}
	~ViewportManager() {}

public:
	void AddViewport(const int &x, const int &y, const int &w, const int &h, char *name);
	ViewportData* GetViewport(char *name);

private:
	std::map<char*, ViewportData> Viewports;
};
