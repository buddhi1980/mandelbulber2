/*
 * settings.hpp
 *
 *  Created on: Jun 20, 2014
 *      Author: krzysztof
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "parameters.hpp"
#include <QtCore>

class cSettings
{
public:
	enum enumFormat
	{
		formatFullText,
		formatCondensedText,
		formatCsv,
		formatAppSettings
	};

	cSettings(enumFormat _format);
	size_t CreateText(const cParameterContainer *par, const cParameterContainer *fractPar);
	bool SaveToFile(QString filename);
	bool LoadFromFile(QString filename);
	bool Decode(cParameterContainer *par, cParameterContainer *fractPar);

private:
	QString CreateHeader(void);
	void DecodeHeader(QStringList &separatedText);
	QString CreateOneLine(const cParameterContainer *par, QString name);
	bool DecodeOneLine(cParameterContainer *par, QString line);
	bool CheckSection(QString text, QString &section);

	enumFormat format;
	QString settingsText;

	bool textPrepared;
	double appVersion;
	double fileVersion;
	QByteArray hash;
};



#endif /* SETTINGS_HPP_ */
