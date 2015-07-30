/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cSettings class - loading / saving settings
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"
#include "animation_frames.hpp"
#include "keyframes.hpp"
#include <QtCore>

class cSettings
{
public:
	enum enumFormat
	{
		formatFullText,
		formatCondensedText,
		formatAppSettings,
		formatNetRender
	};

	cSettings(enumFormat _format);
	size_t CreateText(const cParameterContainer *par, const cFractalContainer *fractPar, cAnimationFrames *frames = NULL, cKeyframes *keyframes = NULL);
	bool SaveToFile(QString filename);
	void SaveToClipboard();
	bool LoadFromFile(QString filename);
	bool LoadFromString(const QString &_settingsText);
	bool LoadFromClipboard();
	bool Decode(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames = NULL, cKeyframes *keyframes = NULL);
	QString GetHashCode() {return hash.toHex();}
	void BeQuiet(bool _quiet) {quiet = _quiet;}
	QString GetSettingsText();

private:
	QString CreateHeader(void);
	void DecodeHeader(QStringList &separatedText);
	QString CreateOneLine(const cParameterContainer *par, QString name);
	bool DecodeOneLine(cParameterContainer *par, QString line);
	bool CheckSection(QString text, QString &section);
	void Compatibility(QString &name, QString &value);
	void CreateAnimationString(QString &text, const QString &headerText, const cAnimationFrames *frames);

	bool DecodeFramesHeader(QString line, cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames);
	bool DecodeFramesLine(QString line, cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames);

	QString everyLocaleDouble(QString txt);

	enumFormat format;
	QString settingsText;

	bool textPrepared;
	bool quiet;
	double appVersion;
	double fileVersion;
	QByteArray hash;
	int csvNoOfColumns;
};



#endif /* SETTINGS_HPP_ */
