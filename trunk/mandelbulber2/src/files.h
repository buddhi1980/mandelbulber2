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

#ifndef FILES_H_
#define FILES_H_

extern "C"
{
#include <png.h>
}


/**********************************************************/
//sources of libraries:
//libpng: http://www.libpng.org/pub/png/libpng.html
/**********************************************************/

#include <string>
#include "cimage.hpp"
#include "system.hpp"
#include <QtCore>

extern std::string logfileName;

std::string IndexFilename(const char* filename, const char* extension, int number);
//METHODDEF(void) my_error_exit(j_common_ptr cinfo);
void SavePNG(QString filename, int width, int height, png_byte *image);
void SavePNG16(QString filename, int width, int height, sRGB16* image16);
void SavePNG16Alpha(QString filename, int width, int height, cImage *image);
void SaveFromTilesPNG16(const char *filename, int width, int height, int tiles);
bool FileIfExists(const char *filename);
int fcopy(const char *source, const char *dest);
void BufferNormalize16(sRGB16 *buffer, unsigned int size);
//void SaveAllImageLayers(const char *filename, cImage *image);
std::string removeFileExtension(const std::string &filename);

bool SaveJPEGQt(QString filename, unsigned char *image, int width, int height, int quality);

#endif /* FILES_H_ */
