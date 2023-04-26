#include <Utilities.h>
#include <GL\glew.h>

void GLEWInitializer::Initialize_GLEW()
{
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW!!\n";
	}

	std::cout << glGetString(GL_VENDOR) << ", Opengl version: " << glGetString(GL_VERSION) << "\n";
}

STLLoader::STLLoader()
{

}

STLLoader::~STLLoader()
{

}

bool STLLoader::load(const std::string& path)
{
	FILE* input;
	fopen_s(&input, path.c_str(), "rb");
	if (!input)
	{
		return false;
	}

	/* from wikipedia:
	* Because ASCII STL files can become very large, a binary version of STL exists. A binary STL file has an 80 character header
	* (which is generally ignored - but which should never begin with 'solid' because that will lead most software to assume that
	* this is an ASCII STL file). Following the header is a 4 byte unsigned integer indicating the number of triangular facets in
	* the file. Following that is data describing each triangle in turn. The file simply ends after the last triangle.
	*
	* Each triangle is described by twelve 32-bit-floating point numbers: three for the normal and then three for the X/Y/Z coordinate
	* of each vertex - just as with the ASCII version of STL. After the twelve floats there is a two byte unsigned 'short' integer that
	* is the 'attribute byte count' - in the standard format, this should be zero because most software does not understand anything else.
	*
	* Floating point numbers are represented as IEEE floating point numbers and the endianness is assumed to be little endian although this
	* is not stated in documentation.
	*/

	// find the file size
	fseek(input, 0, SEEK_END);
	size_t fileSize = ftell(input);
	fseek(input, 0, SEEK_SET);

	//std::vector<char> buffer_vec(fileSize);
	uint8_t* buffer = new uint8_t[fileSize + 1];
	memset(buffer, 0, fileSize + 1);
	size_t num_bytes_read = fread_s(buffer, fileSize + 1, 1, fileSize, input);
	if (num_bytes_read != fileSize)
	{
		fclose(input);
		return false;
	}
	fclose(input);

	return this->load(buffer, num_bytes_read, path);
}

bool STLLoader::load(uint8_t* buffer, const size_t num_bytes, const std::string& origin)
{
	// check for ascii since we can only load binary types with this class
	std::string buffer_str = std::string(reinterpret_cast<char *>(buffer), num_bytes);

	if (buffer_str.substr(0, 5) == std::string("solid"))
	{
		// file says that it is ascii, but why should we trust it?

		// check for "endsolid" as well
		if (buffer_str.find("endsolid", 5) != std::string::npos)
		{
			return false;
		}
	}

	// make sure there's enough data for a binary STL header and triangle count
	static const size_t binary_stl_header_len = 84;
	if (num_bytes <= binary_stl_header_len)
	{
		return false;
	}

	// one last check to make sure that the size matches the number of triangles
	unsigned int num_triangles = *(reinterpret_cast<uint32_t *>(buffer + 80));
	static const size_t number_of_bytes_per_triangle = 50;
	size_t expected_size = binary_stl_header_len + num_triangles * number_of_bytes_per_triangle;
	if (num_bytes < expected_size)
	{
		return false;
	}
	else if (num_bytes > expected_size)
	{

	}

	// load the binary STL data
	return this->load_binary(buffer);
}

bool STLLoader::load_binary(uint8_t* buffer)
{
	uint8_t* pos = buffer;

	pos += 80; // skip the 80 byte header

	unsigned int numTriangles = *(unsigned int*)pos;
	pos += 4;

	for (unsigned int currentTriangle = 0; currentTriangle < numTriangles; ++currentTriangle)
	{
		Vertex v1;
		Vertex v2;
		Vertex v3;

		v1.Normal.X = *(float*)pos;
		v2.Normal.X = *(float*)pos;
		v3.Normal.X = *(float*)pos;

		pos += 4;
		v1.Normal.Y = *(float*)pos;
		v2.Normal.Y = *(float*)pos;
		v3.Normal.Y = *(float*)pos;
		pos += 4;
		v1.Normal.Z = *(float*)pos;
		v2.Normal.Z = *(float*)pos;
		v3.Normal.Z = *(float*)pos;
		pos += 4;

		v1.Position.X = *(float*)pos;
		pos += 4;
		v1.Position.Y = *(float*)pos;
		pos += 4;
		v1.Position.Z = *(float*)pos;
		pos += 4;

		v2.Position.X = *(float*)pos;
		pos += 4;
		v2.Position.Y = *(float*)pos;
		pos += 4;
		v2.Position.Z = *(float*)pos;
		pos += 4;

		v3.Position.X = *(float*)pos;
		pos += 4;
		v3.Position.Y = *(float*)pos;
		pos += 4;
		v3.Position.Z = *(float*)pos;
		pos += 4;

		// Blender was writing a large number into this short... am I misinterpreting what the attribute byte count is supposed to do?
		//unsigned short attributeByteCount = *(unsigned short*)pos;
		pos += 2;

		//pos += attributeByteCount;			

		Vertices.push_back(v1);
		Vertices.push_back(v2);
		Vertices.push_back(v3);			
	}

	std::pair<vec3f, vec3f> p = MeshFactory::CalculateBounds(&Vertices[0], Vertices.size());

	vec3f diff = p.second + p.first;
	diff /= 2.0f;
	
	for (uint32 i = 0; i < Vertices.size(); i++)
	{
		Vertices[i].Position -= diff;
	}
	
	return true;
}

char* LoadTextFile(char *path)
{
	FILE *file;
	fopen_s(&file, path, "r");

	if (!file)
	{
		std::cout << "Failed to load: " << path << std::endl;

		return NULL;
	}

	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *data = new char[length + 1];
	memset(data, 0, length + 1);

	fread_s(data, length + 1, 1, length, file);

	fclose(file);

	return data;
}

char* StringToCharArray(std::string &s)
{
	char *res = new char[s.size() + 1];

	for (uint32 i = 0; i < s.size(); i++)
	{
		res[i] = s[i];
	}

	res[s.size()] = '\0';

	return res;
}

Image ImageLoader::LoadImage(char * path)
{
	Image image = Image{};

	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	
	//pointer to the image, once loaded
	FIBITMAP *dib(0);

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(path, 0);
	
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(path);
	
	//if still unkown, return failure
	if (fif == FIF_UNKNOWN)
		return Image{};
			
	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, path);
	
	//if the image failed to load, return failure
	if (!dib)
		return Image{};

	//retrieve the image data
	//uint8 *data = FreeImage_GetBits(dib);

	//get the image width and height
	image.Width = FreeImage_GetWidth(dib);
	image.Height = FreeImage_GetHeight(dib);
	
	uint32 bytespp = FreeImage_GetLine(dib) / image.Width;
	uint32 imgSize = image.Width * image.Height * bytespp;
	//image.Data = new uint8(imgSize);

	//memcpy(image.Data, data, imgSize);
	image.Data = FreeImage_GetBits(dib);
	
	
	//if this somehow one of these failed (they shouldn't), return failure
	if ((image.Data == 0) || (image.Width == 0) || (image.Height == 0))
		return Image{};

	//FreeImage_Unload(dib);

	return image;
}