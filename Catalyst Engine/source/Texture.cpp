#include <Texture.h>
#include <Utilities.h>

Texture::Texture()
{
	if (TextureID)
	{
		glDeleteTextures(1, &TextureID);
		TextureID = 0;
	}
	else
	{
		TextureID = 0;
	}
}

Texture::Texture(char *path)
{
	if (TextureID)
	{
		glDeleteTextures(1, &TextureID);
		TextureID = 0;
	}
	else
	{
		TextureID = 0;
	}

	CreateTexture(path);
}

void Texture::CreateTexture(char *path)
{
	Image image = ImageLoader::LoadImage(path);

	glGenTextures(1, &TextureID);
	BindTexture();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width, image.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.Data);
	glGenerateMipmap(GL_TEXTURE_2D);

	UnbindTexture();

	//delete image.Data;

	//ImageLoader::UnloadImage(&image);
}

void Texture::BindTexture(int index)
{
	if (TextureID)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, TextureID);
	}
}

void Texture::UnbindTexture()
{
	if (TextureID)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Texture::UnloadTexture()
{
	if (TextureID)
	{
		glDeleteTextures(1, &TextureID);
		TextureID = 0;
	}
	else
	{
		TextureID = 0;
	}
}

bool Texture::IsAvailable()
{
	if (TextureID)
	{
		return true;
	}

	return false;
}
