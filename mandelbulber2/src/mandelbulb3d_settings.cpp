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
		QByteArray settingsBinaryBytes = settingsBinaryString.toLocal8Bit();
		QDataStream in(&settingsBinaryBytes, QIODevice::ReadOnly);
		QDataStream out(&settings, QIODevice::WriteOnly);
		out.setByteOrder(QDataStream::LittleEndian);
		int chunkSize = 4;
		char fourChars[chunkSize];
		while (!in.atEnd())
		{
			in.readRawData(fourChars, chunkSize);
			quint64 parsedLong = FourCharsTo3Bytes(fourChars);
			// qDebug() << parsedLong;
			// from the bytes of the long [0123] only [321] are used and need to be appended
			// out.writeRawData((((char *)&parsedLong) + 1), 3);
			char *parsedLongPtr = (char *) &parsedLong;
			out.writeRawData(parsedLongPtr + 2, 1);
			out.writeRawData(parsedLongPtr + 1, 1);
			out.writeRawData(parsedLongPtr + 0, 1);
			// out.writeRawData(parsedLongPtr + 0, 1);
		}
	}
	// must return a value for msvc Error C4716 must return a value
	return false;
}

void cMandelbulb3dSettings::ConvertToNewContainer(
	cParameterContainer *par, cFractalContainer *fractal)
{
	// general parameters
	par->ResetAllToDefault();

	par->Set("description", settingsTitle);
	// see TMandHeader10 struct here:
	// https://github.com/thargor6/mb3d/blob/66c45c98eb666e1cb0ccb8aaf36bccbb3fc574d2/TypeDefinitions.pas
	// TMandHeader10 = packed record       //Main parameters for storing/loading
	// MandId: Integer;                  //or byte + 3 free bytes

	// Width, Height, Iterations: Integer; // MandId, Iterations could be Word?
	int TMandHeader10Off = 0;
	int mandId = getSettingsInt(TMandHeader10Off);
	par->Set("image_width", getSettingsInt(TMandHeader10Off + 4));
	par->Set("image_width", getSettingsInt(TMandHeader10Off + 8));
	par->Set("max_iter", getSettingsInt(TMandHeader10Off + 12));
/*
		// iOptions: Word;                   // iOptions: SmoothNs: (SpinEdit2.Value shl 6) or FirstStepRandom=bit1 or StepSubDEstop=bit3
		getSettings16Bit(TMandHeader10Off + 16);
		// bNewOptions: Byte;                // bit1: Quaternion instead of RotationMatrix! bit2: color on it nr:
		getSettings8Bit(TMandHeader10Off + 18);
		// bColorOnIt: Byte;                 //0: disabled 1: outputvec:=inputvec (1)2..255 iterate n-1 times + docolor
		getSettings8Bit(TMandHeader10Off + 19);
		// dZstart, dZend: Double;           //#20
		getSettingsDouble(TMandHeader10Off + 20);
		getSettingsDouble(TMandHeader10Off + 28);
		// dXmid, dYmid, dZmid: Double;      //#36
		getSettingsDouble(TMandHeader10Off + 36);
		getSettingsDouble(TMandHeader10Off + 44);
		getSettingsDouble(TMandHeader10Off + 52);*/
	/*qCritical() << "ID: " << mandId << "width: " << par->Get<int>("image_width")
					 << ", height: " << par->Get<int>("image_height");*/
}

int cMandelbulb3dSettings::getSettingsInt(int position){
	bool ok;
	QByteArray a = settings.mid(position, 4);
	int ret = a.at(0);
	// int ret = .toHex().toInt(&ok, 16);
	// if(!ok) qCritical() << "getSettingsInt() error for position: " << position;
	qCritical() << "getSettingsInt() return for position: " << position << " is " << ret;
	return ret;
}

int cMandelbulb3dSettings::getSettings16Bit(int position){
	bool ok;
	int ret = settings.mid(position, 2).toHex().toInt(&ok, 16);
	if(!ok) qCritical() << "getSettings16Bit() error for position: " << position;
	qCritical() << "getSettings16Bit() return for position: " << position << " is " << ret;
	return ret;
}

int cMandelbulb3dSettings::getSettings8Bit(int position){
	bool ok;
	int ret = settings.mid(position, 1).toHex().toInt(&ok, 16);
	if(!ok) qCritical() << "getSettings8Bit() error for position: " << position;
	qCritical() << "getSettings8Bit() return for position: " << position << " is " << ret;
	return ret;
}

double cMandelbulb3dSettings::getSettingsDouble(int position){
	bool ok;
	double ret = 0; // settings.mid(position, 8).toDouble(ok);
	if(!ok) qCritical() << "getSettingsDouble() error for position: " << position;
	qCritical() << "getSettingsDouble() return for position: " << position << " is " << ret;
	return ret;
}

quint64 cMandelbulb3dSettings::FourCharsTo3Bytes(char* fourChars)
{
	// see also mandelbulb implementation here:
	// https://github.com/thargor6/mb3d/blob/66c45c98eb666e1cb0ccb8aaf36bccbb3fc574d2/DivUtils.pas
	quint64 i = 0;
	for (char b = 0; b < 4; b++)
	{
		char c = fourChars[b];
		bool c_46_57 = (c >= 46 && c <= 57);
		bool c_65_90 = (c >= 65 && c <= 90);
		bool c_97_122 = (c >= 97 && c <= 122);
		if(c_46_57 || c_65_90 || c_97_122)
		{
			if (c_97_122)
				c -= 59;
			else if (c_65_90)
				c -= 53;
			else if(c_46_57)
				c -= 46;
			i = (i + c) << (min(1, b - 0) * 6);
		}
	}
	return i;
}
