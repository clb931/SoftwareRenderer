#include "stdafx.h"
#include "ATLAS_Texture.h"


INTERN uint32 texture_count = 0;

namespace ATLAS
{
	Texture::Texture(const char *file_path) : data(nullptr)
	{
		char str[256] = "";
		FILE *pFile;
		BMP colors;

		printf("\nOpenging file...\n");
		fflush(stdout);

		fopen_s(&pFile, file_path, "rb");
		if (pFile) {
			fseek(pFile, sizeof(short) * 9, SEEK_SET);
			fread(&width, sizeof(int), 1, pFile);
			fread(&height, sizeof(int), 1, pFile);
			fseek(pFile, sizeof(short), SEEK_CUR);
			fread(&bpp, sizeof(short), 1, pFile);
			fseek(pFile, sizeof(int) * 6, SEEK_CUR);

			printf("Dimensions: %i x %i\nBPP: %i\n", width, height, bpp);
			fflush(stdout);

			if (bpp <= 8)
				for (uint32 a = 0; a < bpp; a++)
					fseek(pFile, sizeof(int), SEEK_CUR);

			data = (uint8 *)malloc(width * height * 4);
			memset(data, 0, width * height * 4);

			printf("nsize: %.2fKB\n", (width * height) / 1024.0f);
			fflush(stdout);

			uint32 j = 0;
			for (uint32 i = 0; i < width * height; ++i) {
				fread(&colors, sizeof(BMP), 1, pFile);

				data[j + 0] = colors.r;
				data[j + 1] = colors.g;
				data[j + 2] = colors.b;
				data[j + 3] = 255;

				j += 4;
			}

			id = texture_count++;

			printf("Clossing File...\n\n");
			fflush(stdout);

			fclose(pFile);
		}
		else {
			printf("The following error occurred");
			printf("File does not exist.\n");
			fflush(stdout);
		}
	}

	Texture::~Texture()
	{
		printf("Freeing Texture (ID: %i)...\n\n", id);
		fflush(stdout);

		if (data)
			free(data);
	}
}