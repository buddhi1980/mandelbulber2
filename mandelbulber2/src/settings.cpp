/*
 * settings.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: krzysztof
 */

#include "settings.hpp"
#include "system.hpp"

cSettings::cSettings(enumFormat _format)
{
	format = _format;
	settingsText.clear();
	textPrepared = false;
	version = MANDELBULBER_VERSION;
}

size_t cSettings::CreateText(const cParameterContainer *par)
{
	settingsText.clear();
	QList<QString> parameterList = par->GetListOfParameters();

	settingsText += CreateHeader();

	for(int i = 0; i < parameterList.size(); i++)
	{
		settingsText += CreateOneLine(par, parameterList[i]);
	}

	textPrepared = true;
	return settingsText.size();
}

QString cSettings::CreateHeader()
{
	QString header("# Mandelbulber settings file\n");
	header += "# version " + QString::number(version, 'f', 2) + "\n";

	switch(format)
	{
		case fullText:
			header += "# all parameters\n";
			break;
		case condensedText:
			header += "# only used parameters\n";
			break;
		case csv:
			header += "# column separated format\n";
			break;
	}
	return header;
}

QString cSettings::CreateOneLine(const cParameterContainer *par, QString name)
{
	QString text;
	if(format == fullText)
	{
		QString value;
		cParameterContainer::varType type = par->GetVarType(name);
		if(type == cParameterContainer::typeBool)
		{
			bool bValue = par->Get<bool>(name);
			value = bValue ? "true" : "false";
		}
		else
		{
			value = par->Get<QString>(name);
		}
		text = name + " " + value + ";\n";
	}
	return text;
}

bool cSettings::SaveToFile(QString filename)
{
	WriteLogString("Saving settings started", filename);
	QFile qfile(filename);
	if(qfile.open(QIODevice::WriteOnly))
	{
		 QTextStream outstream(&qfile);
		 outstream << settingsText;
		 outstream.flush();
		 qfile.close();
		 return true;
	}
	else
	{
		qCritical() << "Error! File not saved. " << qfile.errorString();
		return false;
	}
}
