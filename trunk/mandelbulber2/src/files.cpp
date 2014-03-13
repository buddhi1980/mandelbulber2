/*********************************************************
 /                   MANDELBULBER
 / functions for operating on files, image loading and
 / saving
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

#include <cstdio>
#include <string.h>
#define PNG_DEBUG 3
//#include <jpeglib.h>
#include "files.h"

using namespace std;

string logfileName;

//***************************** Index filename *************************
//funkcja numerująca pliki
//we:	filename - nazwa pliku bez rozszerzenia
//		extension - rozszerzenie
//		number - numer do dodania
//wy:	fullname - nazwa pliku z numerem i rozszerzeniem
//		return - ilość znaków

std::string IndexFilename(const char* filename, const char* extension, int number)
{
        char tmp[10];
        sprintf(tmp,"%.5i",number);
        return std::string(filename)+tmp+"."+extension;
}

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message)(cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

int LoadJPEG(const char *filename, JSAMPLE *image)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE * infile; /* source file */
	JSAMPARRAY buffer; /* Output row buffer */
	int row_stride; /* physical row width in output buffer */

	if ((infile = fopen(filename, "rb")) == NULL)
	{
		fprintf(stderr, "can't open %s\n", filename);
		return 0;
	}

	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 0;
	}
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */
	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */
	(void) jpeg_read_header(&cinfo, TRUE);

	/* Step 4: set parameters for decompression */

	/* Step 5: Start decompressor */
	(void) jpeg_start_decompress(&cinfo);

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */
	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		//put_scanline_someplace(buffer[0], row_stride);
		memcpy(&image[(cinfo.output_scanline - 1) * row_stride], buffer[0], (int) row_stride);
	}
	/* Step 7: Finish decompression */
	(void) jpeg_finish_decompress(&cinfo);

	/* Step 8: Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return 1;
}

//************************** CheckJPEGsize *******************************
bool CheckJPEGsize(const char *filename, int *width, int *height)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE * infile; /* source file */

	if ((infile = fopen(filename, "rb")) == NULL)
	{
		fprintf(stderr, "can't open %s\n", filename);
		return false;
	}

	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 0;
	}
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */
	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */
	(void) jpeg_read_header(&cinfo, TRUE);

	*width = cinfo.image_width;
	*height = cinfo.image_height;

	/* Step 8: Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return true;
}

//************************** Save JPEG ***********************************
void SaveJPEG(const char *filename, int quality, int width, int height, JSAMPLE *image)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;
	JSAMPROW row_pointer[1];
	long int row_stride;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL)
	{
		fprintf(stderr, "can't open %s\n", filename);
		return;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3; /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = width * 3; /* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &image[(long int)row_stride * cinfo.next_scanline];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
}

void SavePNG(const char *filename, int /*quality*/, int width, int height, png_byte *image)
{
	/* create file */
	FILE *fp = fopen(filename, "wb");
	if (!fp)
	{
		fprintf(stderr, "[write_png_file] File %s could not be opened for writing", filename);
		return;
	}
	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_write_struct failed");
		fclose(fp);
		return;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_info_struct failed");
		fclose(fp);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during init_io");
		fclose(fp);
		return;
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing header");
		fclose(fp);
		return;
	}

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing bytes");
		fclose(fp);
		return;
	}

	png_bytep *row_pointers = new png_bytep[height];
	for (int y = 0; y < height; y++)
	{
		row_pointers[y] = &image[y * width * 3];
	}

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during end of write");
		delete[] row_pointers;
		fclose(fp);
		return;
	}

	png_write_end(png_ptr, NULL);

	delete[] row_pointers;

	fclose(fp);
}

void SaveFromTilesPNG16(const char *filename, int width, int height, int tiles)
{
	/* create file */
	string filenamePNG(filename);
	filenamePNG += "_fromTiles.png";
	FILE *fp = fopen(filenamePNG.c_str(), "wb");
	if (!fp)
	{
		fprintf(stderr, "[write_png_file] File %s could not be opened for writing", filename);
		return;
	}

	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_write_struct failed");
		fclose(fp);
		return;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_info_struct failed");
		fclose(fp);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during init_io");
		fclose(fp);
		return;
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing header");
		fclose(fp);
		return;
	}

	png_set_IHDR(png_ptr, info_ptr, width * tiles, height * tiles, 16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	png_set_swap(png_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing bytes");
		fclose(fp);
		return;
	}

	FILE **files = new FILE*[tiles];

	sRGB16 *rowBuffer = new sRGB16[width * tiles];

	for (int tileRow = 0; tileRow < tiles; tileRow++)
	{
		printf("Compiling image from tiles, row %d\n", tileRow);
		for (int tile = 0; tile < tiles; tile++)
		{
			int fileNumber = tile + tileRow * tiles;
			string filename2 = IndexFilename(filename, "tile", fileNumber);
			files[tile] = fopen(filename2.c_str(), "rb");
		}

		for (int y = 0; y < height; y++)
		{
			for (int tile = 0; tile < tiles; tile++)
			{
				size_t result = fread(&rowBuffer[tile * width], 1, sizeof(sRGB16) * width, files[tile]);
				if (result != sizeof(sRGB16) * width)
				{
					printf("Reading error of image tile files");
					for (int tile2 = 0; tile2 < tiles; tile2++)
						fclose(files[tile2]);
					return;
				}
			}
			png_write_rows(png_ptr, (png_bytep*) &rowBuffer, 1);
		}

		for (int tile = 0; tile < tiles; tile++)
		{
			fclose(files[tile]);
			int fileNumber = tile + tileRow * tiles;
			string filename2 = IndexFilename(filename, "tile", fileNumber);
			remove(filename2.c_str());
		}
	}

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during end of write");
		delete[] rowBuffer;
		fclose(fp);
		return;
	}

	png_write_end(png_ptr, NULL);

	delete[] rowBuffer;
	delete[] files;

	fclose(fp);
}

void SavePNG16(const char *filename, int /*quality*/, int width, int height, sRGB16* image16)
{
	/* create file */
	FILE *fp = fopen(filename, "wb");
	if (!fp)
	{
	 fprintf(stderr, "[write_png_file] File %s could not be opened for writing", filename);
	  return;
	}

	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_write_struct failed");
		fclose(fp);
		return;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_info_struct failed");
		fclose(fp);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during init_io");
		fclose(fp);
		return;
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing header");
		fclose(fp);
		return;
	}

	png_set_IHDR(png_ptr, info_ptr, width, height, 16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	png_set_swap(png_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing bytes");
		fclose(fp);
		return;
	}

	png_bytep *row_pointers = new png_bytep[height];
	for (int y = 0; y < height; y++)
	{
		row_pointers[y] = (png_byte*) &image16[y * width];
	}

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during end of write");
		delete [] row_pointers;
		fclose(fp);
		return;
	}

	png_write_end(png_ptr, NULL);

	delete[] row_pointers;

	fclose(fp);
}

void SavePNG16Alpha(const char *filename, int /*quality*/, int width, int height, cImage *image)
{
	/* create file */
	FILE *fp = fopen(filename, "wb");
	if (!fp)
	{
	   	fprintf(stderr, "[write_png_file] File %s could not be opened for writing", filename);
		return;
	}
	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{ 
		fprintf(stderr, "[write_png_file] png_create_write_struct failed");
		fclose(fp);
		return;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_info_struct failed");
		fclose(fp);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during init_io");
		fclose(fp);
		return;
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing header");
		fclose(fp);
		return;
	}

	png_set_IHDR(png_ptr, info_ptr, width, height, 16, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	png_set_swap(png_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing bytes");
		fclose(fp);
		return;
	}

	png_bytep *row_pointers = new png_bytep[height];
	sImageRGBA16 *image16 = new sImageRGBA16[(unsigned long int)width * height];

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			unsigned long int ptr = x+y*width;
			sRGB16 pixel = image->GetPixelImage16(x,y);
			image16[ptr].R = pixel.R;
			image16[ptr].G = pixel.G;
			image16[ptr].B = pixel.B;
			image16[ptr].A = image->GetPixelAlpha(x,y);
		}
	}

	for (int y = 0; y < height; y++)
	{
		row_pointers[y] = (png_byte*) &image16[y * width];
	}

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during end of write");
		delete[] row_pointers;
		delete[] image16;
		fclose(fp);
		return;
	}

	png_write_end(png_ptr, NULL);

	delete[] row_pointers;
	delete[] image16;

	fclose(fp);
}

bool FileIfExists(const char *filename)
{
	FILE *file;
	file = fopen(filename, "r");
	if (file)
	{
		fclose(file);
		return true;
	}
	else return false;
}

void WriteLog(const char *text)
{
	FILE *logfile = fopen(logfileName.c_str(), "a");
	fprintf(logfile, "%ld: %s\n", (unsigned long int) clock(), text);
	fclose(logfile);
}

void WriteLogDouble(const char *text, double value)
{
	FILE *logfile = fopen(logfileName.c_str(), "a");
	fprintf(logfile, "%ld: %s, value = %g\n", (unsigned long int) clock(), text, value);
	fclose(logfile);
}

int fcopy(const char *source, const char *dest)
{
	// ------ file reading

	FILE * pFile;
	unsigned long lSize;
	char *buffer;
	size_t result;

	pFile = fopen(source, "rb");
	if (pFile == NULL)
	{
		printf("Can't open source file for copying: %s\n", source);
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = new char[lSize];

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		printf("Can't read source file for copying: %s\n", source);
		delete[] buffer;
		fclose(pFile);
		return 2;
	}
	fclose(pFile);

	// ----- file writing

	pFile = fopen(dest, "wb");
	if (pFile == NULL)
	{
		printf("Can't open destination file for copying: %s\n", dest);
		delete[] buffer;
		return 3;
	}
	fwrite(buffer, 1, lSize, pFile);
	fclose(pFile);

	delete [] buffer;
	return 0;
}

void BufferNormalize16(sRGB16 *buffer, unsigned int size)
{
	int maxR = 0;
	int maxG = 0;
	int maxB = 0;
	for(unsigned int i = 0; i<size; i++)
	{
		int R = buffer[i].R; if(R > maxR) maxR = R;
		int G = buffer[i].G; if(G > maxG) maxG = G;
		int B = buffer[i].B; if(B > maxB) maxB = B;
	}
	int max;
	max = (maxR > maxG) ? maxR : maxG;
	max = (maxB > max) ? maxB : max;
	double factor = 65535.0 / max;
	if(max == 0) factor = 1.0;
	for(unsigned int i = 0; i<size; i++)
	{
		buffer[i].R *= factor;
		buffer[i].G *= factor;
		buffer[i].B *= factor;
	}
}

/*
void SaveAllImageLayers(const char *filename, cImage *image)
{
	unsigned int width = image->GetWidth();
	unsigned int height = image->GetHeight();
	unsigned int size = width*height;
	sRGB16 *buffer16 = new sRGB16[width*height];
	sComplexImage *ci = image->GetComplexImagePtr();
	string file(filename);
	file = removeFileExtension(file);
	for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].fogDensity16;
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_fog.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].glowBuf16;
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_glow.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].shadingBuf16;
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_shading.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].shadowsBuf16;
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_shadow.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].specularBuf16;
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_specular.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].ambientBuf16.R; buffer16[i].G = ci[i].ambientBuf16.G; buffer16[i].B = ci[i].ambientBuf16.B;}
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_ambient.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].auxLight.R; buffer16[i].G = ci[i].auxLight.G; buffer16[i].B = ci[i].auxLight.B;}
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_auxLights.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].auxSpecular.R; buffer16[i].G = ci[i].auxSpecular.G; buffer16[i].B = ci[i].auxSpecular.B;}
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_auxlightsSpec.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].backgroundBuf16.R; buffer16[i].G = ci[i].backgroundBuf16.G; buffer16[i].B = ci[i].backgroundBuf16.B;}
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_bkg.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].reflectBuf16.R; buffer16[i].G = ci[i].reflectBuf16.G; buffer16[i].B = ci[i].reflectBuf16.B;}
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_refl.png").c_str(), 100, width, height, buffer16);

	for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].volumetricLight.R; buffer16[i].G = ci[i].volumetricLight.G; buffer16[i].B = ci[i].volumetricLight.B;}
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_fogCol.png").c_str(), 100, width, height, buffer16);

	sImageAdjustments *imageAdjustments = image->GetImageAdjustments();
	unsigned short *colours = image->GetColourIndexBufPtr();
	for (unsigned int i = 0; i < size; i++)
	{
		int colorIndex = colours[i];
		int color_number;
		if (colorIndex >= 248 * 256)
		{
			color_number = colorIndex;
		}
		else
		{
			color_number = (int) (colorIndex * imageAdjustments->coloring_speed + 256 * imageAdjustments->paletteOffset) % 65536;
		}
		sRGB color = image->IndexToColour(color_number);
		buffer16[i].R = color.R; buffer16[i].G = color.G; buffer16[i].B = color.B;
	}
	BufferNormalize16(buffer16, size);
	SavePNG16((file+"_col.png").c_str(), 100, width, height, buffer16);

	unsigned short *alpha = image->GetAlphaBufPtr();
	for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = alpha[i]; buffer16[i].G = alpha[i]; buffer16[i].B = alpha[i];}
	SavePNG16((file+"_alpha.png").c_str(), 100, width, height, buffer16);

	//normalize zBuffer
	double *zbuffer = image->GetZBufferPtr();
	float minZ = 1.0e50;
	float maxZ = 0.0;
	for (unsigned int i = 0; i < size; i++)
	{
		float z = zbuffer[i];
		if(z > maxZ && z < 1e19) maxZ = z;
		if(z < minZ) minZ = z;
	}
	float zRange = maxZ - minZ;
	float zFactor = 65000.0 / zRange;
	for(unsigned int i = 0; i<size; i++)
	{
		int z = (zbuffer[i] - minZ) * zFactor;
		if(zbuffer[i] > 1e19) z = 65535;
		buffer16[i].R = buffer16[i].G = buffer16[i].B = z;
	}
	SavePNG16((file+"_zBuffer.png").c_str(), 100, width, height, buffer16);

	SavePNG16((file+"_image16.png").c_str(), 100, width, height, image->GetImage16Ptr());

	delete [] buffer16;
}
*/


string removeFileExtension(const string &filename)
{
    size_t lastdot = filename.find_last_of(".");
    size_t lastSlash = filename.find_last_of("/");
    size_t lastBackSlash = filename.find_last_of("\\");
    if (lastdot == string::npos)
    {
    	return filename;
    }
    else
    {
    	if((lastdot > lastSlash || lastSlash == string::npos) && (lastdot > lastBackSlash || lastBackSlash == string::npos))
    	{
    		return filename.substr(0, lastdot);
    	}
    	else
    	{
    		return filename;
    	}
    }
}
