/*
 * settings.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: krzysztof
 */

#include "settings.hpp"
#include "system.hpp"
#include "error_message.hpp"
#include <QCryptographicHash>

cSettings::cSettings(enumFormat _format)
{
	format = _format;
	settingsText.clear();
	textPrepared = false;
	appVersion = MANDELBULBER_VERSION;
	fileVersion = 0;
}

size_t cSettings::CreateText(const cParameterContainer *par, const cParameterContainer *fractPar)
{
	settingsText.clear();
	settingsText += CreateHeader();
	settingsText += "[main_parameters]\n";

	QList<QString> parameterList = par->GetListOfParameters();
	for(int i = 0; i < parameterList.size(); i++)
	{
		settingsText += CreateOneLine(par, parameterList[i]);
	}

	for (int f = 0; f < 4; f++)
	{
		settingsText += "[fractal_" + QString::number(f + 1) + "]\n";
		QList<QString> parameterListFractal = fractPar[f].GetListOfParameters();
		for (int i = 0; i < parameterListFractal.size(); i++)
		{
			settingsText += CreateOneLine(&fractPar[f], parameterListFractal[i]);
		}
		parameterListFractal.clear();
	}

	textPrepared = true;

	//hash code will be needed for generating thumbnails
	QCryptographicHash hashCrypt(QCryptographicHash::Md4);
	hashCrypt.addData(settingsText.toUtf8());
	hash = hashCrypt.result();
	//qDebug() << "hash code" << hash.toHex();

	return settingsText.size();
}

QString cSettings::CreateHeader()
{
	QString header("# Mandelbulber settings file\n");
	header += "# version " + QString::number(appVersion, 'f', 2) + "\n";

	switch(format)
	{
		case formatFullText:
			header += "# all parameters\n";
			break;
		case formatCondensedText:
			header += "# only modified parameters\n";
			break;
		case formatCsv:
			header += "# column separated format\n";
			break;
	}
	return header;
}

QString cSettings::CreateOneLine(const cParameterContainer *par, QString name)
{
	QString text;
	if ((format != formatAppSettings && par->GetParameterType(name) == paramStandard) || (format == formatAppSettings && par->GetParameterType(name) == paramApp))
	{
		if (format == formatFullText || format == formatCondensedText)
		{
			QString value;
			cParameterContainer::enumVarType type = par->GetVarType(name);
			if (!par->isDefaultValue(name) || format == formatFullText)
			{
				if (type == cParameterContainer::typeBool)
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
		}
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
		cErrorMessage::showMessage(QString("Settings file not saved!\n") + filename + "\n" + qfile.errorString(), cErrorMessage::errorMessage);
		return false;
	}
}

bool cSettings::LoadFromFile(QString filename)
{
	settingsText.clear();
	textPrepared = false;
	WriteLogString("Loading settings started", filename);
	QFile qfile(filename);
	if(qfile.open(QIODevice::ReadOnly))
	{
		QTextStream instream(&qfile);
		settingsText.append(instream.readAll());
		qfile.close();
		textPrepared = true;

		//hash code will be needed for generating thumbnails
		QCryptographicHash hashCrypt(QCryptographicHash::Md4);
		hashCrypt.addData(settingsText.toUtf8());
		hash = hashCrypt.result();
		//qDebug() << "hash code" << hash.toHex();

		return true;
	}
	else
	{
		if(!quiet)
		{
			cErrorMessage::showMessage(QString("Settings file not loaded!\n") + filename + "\n" + qfile.errorString(), cErrorMessage::errorMessage);
		}
		return false;
	}
}

void cSettings::DecodeHeader(QStringList &separatedText)
{
	if (textPrepared)
	{
		try
		{
			int size = separatedText.size();
			if (size < 3)
				throw QString("It's not valid Mandelbulber settings file. No header");

			QString firstLine = separatedText[0];

			if (!firstLine.contains("Mandelbulber settings file"))
				throw QString("It's not valid Mandelbulber settings file. Wrong header");

			QString secondLine = separatedText[1];
			int versionPos = secondLine.lastIndexOf("version");

			if (versionPos < 0)
				throw QString("It's not valid Mandelbulber settings file. No information about version of file");

			int numberPos = versionPos + QString("version").length();

			bool ok;
			fileVersion = secondLine.mid(numberPos).toDouble(&ok);

			if(!ok)
				throw QString("It's not valid Mandelbulber settings file. Wrong file version number");

			if (fileVersion > appVersion)
				throw QString("File was saved in new version of Mandelbulber\nFile version: ") + QString::number(fileVersion);

			QString thirdLine = separatedText[2];
			if(thirdLine.contains("all parameters"))
			{
				format = formatFullText;
			}
			else if(thirdLine.contains("only modified parameters"))
			{
				format = formatCondensedText;
			}
			else if(thirdLine.contains("column separated format"))
			{
				format = formatCsv;
			}
			else
			{
				throw QString("It's not valid Mandelbulber settings file. Format not specified in the header");
			}

		}
		catch (QString &error)
		{
			cErrorMessage::showMessage(error, cErrorMessage::errorMessage);
			textPrepared = false;
			return;
		}
	}
}


bool cSettings::Decode(cParameterContainer *par, cParameterContainer *fractPar)
{
	//clear settings
	par->ResetAllToDefault();
	for(int i=0; i<4; i++)
		fractPar[i].ResetAllToDefault();

	QStringList separatedText = settingsText.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
	DecodeHeader(separatedText);

	int errorCount = 0;

	QString section;
	if(textPrepared)
	{
		for(int l = 3; l < separatedText.size(); l++)
		{
			QString line = separatedText[l];
			bool isNewSection = CheckSection(line, section);
			if(!isNewSection)
			{
				bool result = false;
				if(section == QString("main_parameters"))
				{
					result = DecodeOneLine(par, line);
				}
				else if(section.contains("fractal"))
				{
					int i = section.right(1).toInt() - 1;
					result = DecodeOneLine(&fractPar[i], line);
				}

				if (!result)
				{
					QString errorMessage = QString("Error in settings file. Line: ") + QString::number(l) + " (" + line + ")";
					cErrorMessage::showMessage(errorMessage, cErrorMessage::errorMessage);
					errorCount++;
					if(errorCount > 3)
					{
						cErrorMessage::showMessage("Too many errors in settings file", cErrorMessage::errorMessage);
						return false;
					}
				}

			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool cSettings::DecodeOneLine(cParameterContainer *par, QString line)
{
	int firstSpace = line.indexOf(' ');
	int semicolon = line.indexOf(';');
	QString parameterName = line.left(firstSpace);
	QString value = line.mid(firstSpace + 1, semicolon - firstSpace - 1);

	cParameterContainer::enumVarType varType = par->GetVarType(parameterName);
	if(varType == cParameterContainer::null)
	{
		return false;
	}
	else
	{
		if(varType == cParameterContainer::typeBool)
		{
			value = (value == QString("true")) ? "1" : "0";
		}
		par->Set(parameterName, value);
		return true;
	}
}

bool cSettings::CheckSection(QString text, QString &section)
{
	if(text.left(1) == "[")
	{
		section = text.mid(1, text.length()-2);
		return true;
	}
	return false;
}

