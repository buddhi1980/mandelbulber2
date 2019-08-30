/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * file operation functions
 */

#include "files.h"

#include <cstdio>
#include <cstring>

#include "cimage.hpp"
#include "error_message.hpp"
#include "initparameters.hpp"

// custom includes
extern "C"
{
#include <png.h>
}

using namespace std;

string logfileName;

//***************************** Index filename *************************
// function for numerating file names
// in:	filename - file name without extension
//		extension
//		number - number to apply
// out:	fullname - file name with added number
std::string IndexFilename(const char *filename, const char *extension, int number)
{
	char tmp[10];
	sprintf(tmp, "%.5i", number);
	return std::string(filename) + tmp + "." + extension;
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
	else
		return false;
}

int fcopy(const char *source, const char *dest)
{
	// ------ file reading

	FILE *pFile;
	size_t lSize;
	char *buffer;
	size_t result;

	pFile = fopen(source, "rb");
	if (pFile == nullptr)
	{
		qCritical() << "Can't open source file for copying: " << source;
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	if (lSize > 0)
	{
		// allocate memory to contain the whole file:
		buffer = new char[lSize];

		// copy the file into the buffer:
		result = fread(buffer, 1, lSize, pFile);
		if (result != size_t(lSize))
		{
			qCritical() << "Can't read source file for copying: " << source;
			delete[] buffer;
			fclose(pFile);
			return 2;
		}
	}
	else
	{
		qCritical() << "Can't obtain file size: " << source;
		fclose(pFile);
		return 4;
	}
	fclose(pFile);

	// ----- file writing

	pFile = fopen(dest, "wb");
	if (pFile == nullptr)
	{
		qCritical() << "Can't open destination file for copying: " << dest;
		delete[] buffer;
		return 3;
	}
	fwrite(buffer, 1, lSize, pFile);
	fclose(pFile);

	delete[] buffer;
	return 0;
}

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
		if ((lastdot > lastSlash || lastSlash == string::npos)
				&& (lastdot > lastBackSlash || lastBackSlash == string::npos))
		{
			return filename.substr(0, lastdot);
		}
		else
		{
			return filename;
		}
	}
}

void BufferNormalize16(sRGB16 *buffer, unsigned int size)
{
	int maxR = 0;
	int maxG = 0;
	int maxB = 0;
	for (unsigned int i = 0; i < size; i++)
	{
		int R = buffer[i].R;
		if (R > maxR) maxR = R;
		int G = buffer[i].G;
		if (G > maxG) maxG = G;
		int B = buffer[i].B;
		if (B > maxB) maxB = B;
	}
	int max;
	max = (maxR > maxG) ? maxR : maxG;
	max = (maxB > max) ? maxB : max;
	double factor = 1.0;
	if (max > 0) factor = 65535.0 / max;
	for (unsigned int i = 0; i < size; i++)
	{
		buffer[i].R *= factor;
		buffer[i].G *= factor;
		buffer[i].B *= factor;
	}
}

QStringList SaveImage(QString filename, ImageFileSave::enumImageFileType fileType, cImage *image,
	QObject *updateReceiver)
{
	QStringList listOfSavedFiles;

	ImageFileSave::ImageConfig imageConfig;
	QStringList imageChannelNames = ImageFileSave::ImageChannelNames();

	// read image config from preferences
	for (int i = 0; i < imageChannelNames.size(); i++)
	{
		QString imageChannelName = imageChannelNames.at(i);
		if (gPar->Get<bool>(imageChannelName + "_enabled"))
		{
			ImageFileSave::enumImageContentType contentType = ImageFileSave::enumImageContentType(i);
			ImageFileSave::enumImageChannelQualityType channelQuality =
				ImageFileSave::enumImageChannelQualityType(gPar->Get<int>(imageChannelName + "_quality"));
			QString postfix = gPar->Get<QString>(imageChannelName + "_postfix");
			imageConfig.insert(
				contentType, ImageFileSave::structSaveImageChannel(contentType, channelQuality, postfix));
		}
	}

	if (image->IsStereoLeftRight() && gPar->Get<bool>("stereoscopic_in_separate_files"))
	{
		cImage *leftImage = new cImage(1, 1, true);
		cImage *rightImage = new cImage(1, 1, true);
		image->GetStereoLeftRightImages(leftImage, rightImage);

		// save left
		{
			QString fileWithoutExtension = ImageFileSave::ImageNameWithoutExtension(filename) + "_left";
			ImageFileSave *imageFileSave =
				ImageFileSave::create(fileWithoutExtension, fileType, leftImage, imageConfig);
			if (updateReceiver != nullptr)
			{
				QObject::connect(imageFileSave,
					SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), updateReceiver,
					SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
			}
			QStringList list = imageFileSave->SaveImage();
			listOfSavedFiles.append(list);

			delete imageFileSave;
		}

		// save right
		{
			QString fileWithoutExtension = ImageFileSave::ImageNameWithoutExtension(filename) + "_right";
			ImageFileSave *imageFileSave =
				ImageFileSave::create(fileWithoutExtension, fileType, rightImage, imageConfig);
			if (updateReceiver != nullptr)
			{
				QObject::connect(imageFileSave,
					SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), updateReceiver,
					SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
			}
			QStringList list = imageFileSave->SaveImage();
			listOfSavedFiles.append(list);

			delete imageFileSave;
		}

		delete leftImage;
		delete rightImage;
	}
	else
	{
		QString fileWithoutExtension = ImageFileSave::ImageNameWithoutExtension(filename);
		ImageFileSave *imageFileSave =
			ImageFileSave::create(fileWithoutExtension, fileType, image, imageConfig);
		if (updateReceiver != nullptr)
		{
			QObject::connect(imageFileSave,
				SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), updateReceiver,
				SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
		}
		QStringList list = imageFileSave->SaveImage();
		listOfSavedFiles.append(list);

		delete imageFileSave;
	}

	return listOfSavedFiles;
}

sRGBA16 *LoadPNG(QString filename, int &outWidth, int &outHeight)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 8;
	int color_type, interlace_type;
	FILE *fp;

	if ((fp = fopen(filename.toLocal8Bit().constData(), "rb")) == nullptr) return nullptr;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (png_ptr == nullptr)
	{
		fclose(fp);
		return nullptr;
	}

	uchar sig[8];
	/* first do a quick check that the file really is a PNG image; could
	 * have used slightly more general png_sig_cmp() function instead */
	size_t bytesRead = fread(sig, 1, 8, fp);
	if (bytesRead < 8)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return nullptr;
	}

	if (!png_check_sig(sig, 8))
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return nullptr; /* bad signature */
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == nullptr)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return nullptr;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(fp);
		return nullptr;
	}

	png_init_io(png_ptr, fp);

	png_set_sig_bytes(png_ptr, sig_read);

	png_read_png(png_ptr, info_ptr,
		PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_SWAP_ENDIAN, nullptr);

	png_uint_32 width, height;
	int bit_depth;
	png_get_IHDR(
		png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, nullptr, nullptr);
	outWidth = width;
	outHeight = height;
	// unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	// qDebug() << width << height << bit_depth << color_type << row_bytes;

	sRGBA16 *image = new sRGBA16[outWidth * outHeight];

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	if (bit_depth == 8)
	{
		if (color_type == PNG_COLOR_TYPE_RGB)
		{
			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					unsigned char *pointer = static_cast<unsigned char *>(row_pointers[y]) + x * 3;
					sRGBA16 pixel(pointer[0] * 256, pointer[1] * 256, pointer[2] * 256, 65535);
					image[x + y * outWidth] = pixel;
				}
			}
		}
		else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		{
			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					unsigned char *pointer = static_cast<unsigned char *>(row_pointers[y]) + x * 4;
					sRGBA16 pixel(pointer[0] * 256, pointer[1] * 256, pointer[2] * 256, pointer[2] * 256);
					image[x + y * outWidth] = pixel;
				}
			}
		}
		if (color_type == PNG_COLOR_TYPE_GRAY)
		{
			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					unsigned char *pointer = static_cast<unsigned char *>(row_pointers[y]) + x;
					sRGBA16 pixel(pointer[0] * 256, pointer[0] * 256, pointer[0] * 256, 65535);
					image[x + y * outWidth] = pixel;
				}
			}
		}
	}
	else if (bit_depth == 16)
	{
		if (color_type == PNG_COLOR_TYPE_RGB)
		{
			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					unsigned short *pointer = reinterpret_cast<unsigned short *>(row_pointers[y]) + x * 3;
					sRGBA16 pixel(pointer[0], pointer[1], pointer[2], 65535);
					image[x + y * outWidth] = pixel;
				}
			}
		}
		else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		{
			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					unsigned short *pointer = reinterpret_cast<unsigned short *>(row_pointers[y]) + x * 4;
					sRGBA16 pixel(pointer[0], pointer[1], pointer[2], pointer[3]);
					image[x + y * outWidth] = pixel;
				}
			}
		}
		if (color_type == PNG_COLOR_TYPE_GRAY)
		{
			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					unsigned short *pointer = reinterpret_cast<unsigned short *>(row_pointers[y]) + x;
					sRGBA16 pixel(pointer[0], pointer[0], pointer[0], 65535);
					image[x + y * outWidth] = pixel;
				}
			}
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	fclose(fp);

	return image;
}

bool FileExists(const QString &path)
{
	QFileInfo check_file(path);
	return check_file.exists() && check_file.isFile();
}

QString FilePathHelper(const QString &path, const QStringList &pathList)
{
	// QString newPath;

	// original file was found
	if (FileExists(path)) return path;

	WriteLogCout("File " + path + " not found. Looking for the file in alternate locations\n", 2);

	foreach (QString alternatePath, pathList)
	{
		WriteLogCout("Looking for the file at " + alternatePath + "\n", 2);
		if (FileExists((alternatePath)))
		{
			WriteLogCout("File found at " + alternatePath + "\n", 2);
			return alternatePath;
		}
	}

	qWarning() << QString("Substitute path for %1 not found anywhere").arg(path);
	return path;
}

QString FilePathHelperTextures(const QString &path)
{
	QString nativePath = path;
	nativePath = nativePath.replace(QChar('\\'), QChar('/'));
	nativePath = QDir::toNativeSeparators(nativePath);
	QFileInfo fi(nativePath);
	QString fileName = fi.fileName();

	QStringList pathList = {fileName, systemData.homeDir + "textures" + QDir::separator() + fileName,
		systemData.sharedDir + "textures" + QDir::separator() + fileName,
		systemData.homeDir + "mandelbulber2" + QDir::separator() + "textures" + QDir::separator()
			+ fileName,
		systemData.sharedDir + "mandelbulber2" + QDir::separator() + "textures" + QDir::separator()
			+ fileName,
		systemData.GetDataDirectoryUsed() + "textures" + QDir::separator() + fileName,
		systemData.GetDataDirectoryUsed() + fileName,
		QFileInfo(systemData.lastSettingsFile).path() + QDir::separator() + fileName};

	return FilePathHelper(nativePath, pathList);
}

QString FilePathHelperSounds(const QString &path)
{
	QString nativePath = path;
	nativePath = nativePath.replace(QChar('\\'), QChar('/'));
	nativePath = QDir::toNativeSeparators(nativePath);
	QFileInfo fi(nativePath);
	QString fileName = fi.fileName();

	QStringList pathList = {fileName, systemData.homeDir + "sounds" + QDir::separator() + fileName,
		systemData.sharedDir + "sounds" + QDir::separator() + fileName,
		systemData.homeDir + "mandelbulber2" + QDir::separator() + "sounds" + QDir::separator()
			+ fileName,
		systemData.sharedDir + "mandelbulber2" + QDir::separator() + "sounds" + QDir::separator()
			+ fileName,
		systemData.GetDataDirectoryUsed() + "sounds" + QDir::separator() + fileName,
		systemData.GetDataDirectoryUsed() + fileName,
		QFileInfo(systemData.lastSettingsFile).path() + QDir::separator() + fileName};

	return FilePathHelper(nativePath, pathList);
}

QByteArray LoadUtf8TextFromFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCritical() << "Cant open text file: " << fileName << " error: " << file.errorString();
		return QByteArray();
	}
	else
	{
		QTextStream in(&file);
		QString text;
		text = in.readAll();
		file.close();
		return text.toUtf8();
	}
}

QString AnimatedFileName(
	const QString &filenameString, int frame, const QList<QString> *netRenderTextureList)
{
	QString outFilename = filenameString;
	if (filenameString.contains('%'))
	{
		int firstPercent = filenameString.indexOf('%');
		int numberOfPercents = 1;

		for (int i = firstPercent; i < filenameString.length(); i++)
		{
			if (filenameString.at(i) != '%')
			{
				numberOfPercents = i - firstPercent;
				break;
			}
		}

		QString numberString = QString("%1").arg(frame, numberOfPercents, 10, QChar('0'));

		for (int i = firstPercent; i < firstPercent + numberOfPercents; i++)
		{
			outFilename[i] = numberString[i - firstPercent];
		}

		// looking for the texture in NetRender
		if (netRenderTextureList)
		{
			if (netRenderTextureList->length() > 0)
			{
				int maxLookupIndex = pow(10, numberOfPercents);

				for (int index = 0; index < maxLookupIndex; index++)
				{
					QString tempFilename = filenameString;
					for (int i = firstPercent; i < firstPercent + numberOfPercents; i++)
					{
						QString tempNumberString = QString("%1").arg(index, numberOfPercents, 10, QChar('0'));
						tempFilename[i] = tempNumberString[i - firstPercent];
					}

					if (netRenderTextureList->contains(tempFilename))
					{
						return tempFilename;
					}
				}
			}
		}
		else
		{
			// looking for last file number and make sequence as a loop
			if (!QFile::exists(outFilename))
			{
				QFileInfo fileInfo(outFilename);
				QDir dir = fileInfo.absoluteDir();

				QStringList nameFilters;
				QString nameFilter = QFileInfo(filenameString).fileName();
				nameFilter = nameFilter.replace(QChar('%'), QChar('?'));
				nameFilters.append(nameFilter);
				dir.setNameFilters(nameFilters);
				QStringList fileList = dir.entryList(QDir::Files, QDir::Name);

				if (fileList.length() > 0)
				{
					QString lastFile = fileList.last();
					QString maxIndexText = lastFile.mid(
						firstPercent - filenameString.length() + lastFile.length(), numberOfPercents);
					int maxIndex = maxIndexText.toInt();

					int frameModulo = frame % (maxIndex + 1);

					// correct frame number
					numberString = QString("%1").arg(frameModulo, numberOfPercents, 10, QChar('0'));
					for (int i = firstPercent; i < firstPercent + numberOfPercents; i++)
					{
						outFilename[i] = numberString[i - firstPercent];
					}
				}
			}
		}
	}

	return outFilename;
}
