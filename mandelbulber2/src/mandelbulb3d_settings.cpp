/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * cMandelbulb3dSettings class - importing settings from Mandelbulb3d
 */

#include "mandelbulb3d_settings.hpp"

#include "camera_target.hpp"
#include "error_message.hpp"
#include "files.h"
#include "fractal_container.hpp"
#include "fractal_list.hpp"
#include "fractparams.hpp"
#include "initparameters.hpp"
#include "system.hpp"

cMandelbulb3dSettings::cMandelbulb3dSettings() = default;

cMandelbulb3dSettings::~cMandelbulb3dSettings() = default;

bool cMandelbulb3dSettings::LoadSettings(const QString &filename)
{
	QByteArray settingsString = LoadUtf8TextFromFile(filename);
	settingsString = settingsString.replace("\r", "");
	settingsString = settingsString.replace("\n", "");
	settingsString = settingsString.trimmed();

	qWarning() << settingsString;

	QRegularExpression reType("Mandelbulb3Dv18{(.+?)}\s*{Titel:(.*)}");
	QRegularExpressionMatch matchType = reType.match(settingsString);
	if (matchType.hasMatch())
	{
		settingsTitle = matchType.captured(2);
		settingsBinaryString = matchType.captured(1);
	}
	else
	{
		QRegularExpression reType2("Mandelbulb3Dv18{(.+?)}");
		QRegularExpressionMatch matchType2 = reType2.match(settingsString);
		if (matchType2.hasMatch())
		{
			settingsBinaryString = matchType2.captured(1);
		}
		else
		{
			qWarning() << "error parsing mandelbulb 3d file.";
		}
	}

	if (settingsBinaryString != "")
	{
		QDataStream out(&settings, QIODevice::WriteOnly);
		out.setByteOrder(QDataStream::LittleEndian); // this is windows!
		for (int i = 0; i < settingsBinaryString.size(); i += 4)
		{
			QString fourChars = settingsBinaryString.mid(i, 4);
			long parsedLong = FourCharsTo3Bytes(fourChars);
			// qDebug() << parsedLong;
			// from the bytes of the long [0123] only [123] are used and need to be appended
			out.writeRawData((((char *)&parsedLong) + 1), 3);
		}
	}
}

void cMandelbulb3dSettings::ConvertToNewContainer(
	cParameterContainer *par, cFractalContainer *fractal) const
{
	// general parameters
	par->ResetAllToDefault();

	par->Set("description", settingsTitle);
	// see TMandHeader10 struct here:
	// https://github.com/thargor6/mb3d/blob/66c45c98eb666e1cb0ccb8aaf36bccbb3fc574d2/TypeDefinitions.pas
	// TMandHeader10 = packed record       //Main parameters for storing/loading
	// MandId: Integer;                  //or byte + 3 free bytes

	// Width, Height, Iterations: Integer; // MandId, Iterations could be Word?
	/* TODO bool ok;
	int a = settings.mid(0, 4).toHex().toInt(&ok, 16);
	int b = settings.mid(8, 4).toHex().toInt(&ok, 16);
	int c = settings.mid(12, 4).toHex().toInt(&ok, 16);
	par->Set("image_width", settings.mid(4, 4).toInt());
	par->Set("image_width", settings.mid(8, 4).toInt()); */
}

long cMandelbulb3dSettings::FourCharsTo3Bytes(QString fourChars)
{
	// see also mandelbulb implementation here:
	// https://github.com/thargor6/mb3d/blob/66c45c98eb666e1cb0ccb8aaf36bccbb3fc574d2/DivUtils.pas
	long i = 0;
	const char *rawBytes = fourChars.toLatin1().toStdString().c_str();
	for (char b = 0; b < 4; b++)
	{
		char c = rawBytes[b];
		if ((c >= 46 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122))
		{
			if (c > 96)
				c -= 59;
			else if (c > 64)
				c -= 53;
			else
				c -= 46;
			i += c << ((b - 0) * 6);
		}
	}
	return i;
}
