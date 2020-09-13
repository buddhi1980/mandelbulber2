/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "thumbnail.hpp"

#include <utility>

#include <QDir>
#include <QFileInfo>
#include <QPixmap>

#include "cimage.hpp"
#include "file_image.hpp"
#include "render_job.hpp"
#include "rendering_configuration.hpp"
#include "settings.hpp"
#include "system_directories.hpp"

cThumbnail::cThumbnail(const std::shared_ptr<cParameterContainer> _params,
	const std::shared_ptr<cFractalContainer> _fractal, int _width, int _height,
	QString _hash = QString())
		: params(_params), fractal(_fractal), width(_width), height(_height), hash(std::move(_hash))
{
	image = nullptr;
	qWidget = nullptr;
	image.reset(new cImage(width, height));
	// image->CreatePreview(1.0, width, height, qWidget);
}

cThumbnail::~cThumbnail()
{
	// nothing to delete
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
		systemDirectories.GetThumbnailsFolder() + QDir::separator() + hash + QString(".png");
	if (QFileInfo::exists(thumbnailFileName))
	{
		pixmap.load(thumbnailFileName);
	}
	else
	{
		bool stopRequest = false;
		std::unique_ptr<cRenderJob> renderJob(
			new cRenderJob(params, fractal, image, &stopRequest, qWidget));
		renderJob->UseSizeFromImage(true);

		cRenderingConfiguration config;
		config.DisableProgressiveRender();
		config.DisableRefresh();
		config.DisableProgressiveRender();
		config.EnableIgnoreErrors();

		renderJob->Init(cRenderJob::still, config);
		renderJob->Execute();
		QImage qImage(static_cast<const uchar *>(image->ConvertTo8bitChar()), width, height,
			width * sizeof(sRGB8), QImage::Format_RGB888);
		pixmap.convertFromImage(qImage);
		pixmap.save(thumbnailFileName, "PNG");
	}
	return pixmap;
}

void cThumbnail::Save(QString filename) const
{
	ImageFileSaveJPG::SaveJPEGQt(
		filename, image->ConvertTo8bitChar(), image->GetWidth(), image->GetHeight(), 85);
}
