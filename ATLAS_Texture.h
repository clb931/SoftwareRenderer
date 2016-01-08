#ifndef ATLAS_TEXTURE_H


namespace ATLAS
{
	class Texture
	{
	public:
		Texture(const char *file_path);
		~Texture();

		uint8 *data;
		uint32 width;
		uint32 height;
		uint32 bpp;
		uint32 id;

	private:
		struct BMP{ uint8 b, g, r; };
	};
}


#define ATLAS_TEXTURE_H
#endif