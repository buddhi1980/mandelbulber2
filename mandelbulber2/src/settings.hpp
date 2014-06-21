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
		fullText,
		condensedText,
		csv
	};

	cSettings(enumFormat _format);
	size_t CreateText(const cParameterContainer *par);
	bool SaveToFile(QString filename);

private:
	QString CreateHeader(void);
	QString CreateOneLine(const cParameterContainer *par, QString name);

	enumFormat format;
	QString settingsText;
	bool textPrepared;
	double version;
};



#endif /* SETTINGS_HPP_ */
