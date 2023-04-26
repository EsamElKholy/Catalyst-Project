#pragma once

#include <iostream>
#include <string>
#include <iostream>
#include <vector>
#include <FreeImage\FreeImage.h>

#include <cinder\CinderMath.h>

struct Vertex;

class STLLoader
{
public:
	STLLoader();
	~STLLoader();

	bool load(const std::string& path);
	bool load(uint8_t* buffer, const size_t num_bytes, const std::string& origin);

protected:
	bool load_binary(uint8_t* buffer);

public:
	std::vector<Vertex> Vertices;
};

class GLEWInitializer
{
public:
	~GLEWInitializer() {}
private:
	GLEWInitializer() {}

public:
	static void Initialize_GLEW();
};

struct Image 
{
	uint8 *Data;
	uint32 Width, Height;
};

class ImageLoader 
{
public:
	~ImageLoader() {}

private:
	ImageLoader() {}
	ImageLoader(const ImageLoader &im) = default;
	ImageLoader& operator=(const ImageLoader &im) = default;

public:
	static Image LoadImage(char *path);
};

char* LoadTextFile(char *path);

char* StringToCharArray(std::string &s);