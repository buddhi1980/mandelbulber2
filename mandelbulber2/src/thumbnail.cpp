/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cThumbnail class - thumbnail rendering
 */

#include <QFileInfo>
#include <QPixmap>

#include "thumbnail.hpp"
#include "cimage.hpp"
#include "file_image.hpp"
#include "render_job.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "rendering_configuration.hpp"

cThumbnail::cThumbnail(const cParameterContainer *_params, const cFractalContainer *_fractal,
	int _width, int _height, const QString &_hash = QString())
		: params(_params), fractal(_fractal), width(_width), height(_height), hash(_hash)
{
	image = nullptr;
	qwidget = nullptr;
	image = new cImage(width, height);
	// image->CreatePreview(1.0, width, height, qwidget);
}

cThumbnail::~cThumbnail()
{
	delete image;
}

QPixmap cThumbnail::Render()
{
	QPixmap pixmap;

	if (hash.isEmpty())
	{
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(params, fractal);
		hash = tempSettings.GetHashCode();
	}

	QString thumbnailFileName =
		systemData.GetThumbnailsFolder() + QDir::separator() + hash + QString(".png");
	if (QFileInfo::exists(thumbnailFileName))
	{
		pixmap.load(thumbnailFileName);
	}
	else
	{
		bool stopRequest = false;
		cRenderJob *renderJob = new cRenderJob(params, fractal, image, &stopRequest, qwidget);
		renderJob->UseSizeFromImage(true);

		cRenderingConfiguration config;
		config.DisableProgressiveRender();
		config.DisableRefresh();
		config.DisableProgressiveRender();
		config.EnableIgnoreErros();

		renderJob->Init(cRenderJob::still, config);
		renderJob->Execute();
		QImage qimage(static_cast<const uchar *>(image->ConvertTo8bit()), width, height,
			width * sizeof(sRGB8), QImage::Format_RGB888);
		pixmap.convertFromImage(qimage);
		delete renderJob;
		pixmap.save(thumbnailFileName, "PNG");
	}
	return pixmap;
}

void cThumbnail::Save(QString filename) const
{
	ImageFileSaveJPG::SaveJPEGQt(
		filename, image->ConvertTo8bit(), image->GetWidth(), image->GetHeight(), 85);
}
