/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cSettings class - loading / saving settings
 *
 * cSettings can transpose program internal settings to settings string and vice versa.
 * It has also methods to [load / save] [from / to] [clipboard / string / file]
 */

#include "settings.hpp"

#include <QClipboard>
#include <QCryptographicHash>

#include "animation_frames.hpp"
#include "error_message.hpp"
#include "fractal_container.hpp"
#include "fractal_enums.h"
#include "initparameters.hpp"
#include "keyframes.hpp"
#include "light.h"
#include "lights.hpp"
#include "material.h"
#include "primitives.h"
#include "projection_3d.hpp"
#include "system.hpp"
#include "system_data.hpp"
#include "write_log.hpp"

cSettings::cSettings(enumFormat _format)
{
	format = _format;
	settingsText.clear();
	textPrepared = false;
	appVersion = MANDELBULBER_VERSION;
	fileVersion = 0;
	quiet = false;
	csvNoOfColumns = 0;
	foundAnimSoundParameters = false;
	forcedFractalFormulaIndex = -1;
}

size_t cSettings::CreateText(std::shared_ptr<const cParameterContainer> par,
	std::shared_ptr<const cFractalContainer> fractPar, std::shared_ptr<cAnimationFrames> frames,
	std::shared_ptr<cKeyframes> keyframes)
{
	WriteLog("Create settings text", 3);
	settingsText.clear();
	QString header = CreateHeader();
	settingsText += header;
	if ((format == formatFullText || format == formatCondensedText) && par->IfExists("description")
			&& par->Get<QString>("description") != "")
	{
		settingsText += "[description]\n";
		settingsText += par->Get<QString>("description") + "\n";
	}
	settingsText += "[main_parameters]\n";

	// standard parameters
	QList<QString> parameterList = par->GetListOfParameters();
	for (auto &parameterNameFromList : parameterList)
	{
		if (parameterNameFromList == "description") continue;

		if (!listOfParametersToProcess.isEmpty()) // selective saving
		{
			if (!listOfParametersToProcess.contains(QString("main_") + parameterNameFromList)) continue;
		}
		settingsText += CreateOneLine(par, parameterNameFromList);
	}

	if (format != formatAppSettings)
	{
		if (fractPar)
		{
			for (int f = 0; f < NUMBER_OF_FRACTALS; f++)
			{
				QList<QString> parameterListFractal = fractPar->at(f)->GetListOfParameters();
				QString fractalSettingsText = "";
				for (const auto &parameterNameFromFractal : parameterListFractal)
				{
					if (!listOfParametersToProcess.isEmpty()) // selective saving
					{
						if (!listOfParametersToProcess.contains(
									QString("fractal%1_").arg(f) + parameterNameFromFractal))
							continue;
					}
					fractalSettingsText += CreateOneLine(fractPar->at(f), parameterNameFromFractal);
				}
				if (fractalSettingsText.length() > 0)
				{
					settingsText += "[fractal_" + QString::number(f + 1) + "]\n";
					settingsText += fractalSettingsText;
				}
				parameterListFractal.clear();
			}
		}

		if (listOfParametersToProcess.isEmpty())
		{
			// flight animation
			CreateAnimationString(settingsText, QString("frames"), frames);

			// keyframe animation
			CreateAnimationString(settingsText, QString("keyframes"), keyframes);
		}
	}
	textPrepared = true;

	// hash code will be needed for generating thumbnails
	QCryptographicHash hashCrypt(QCryptographicHash::Md4);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QStringRef settingTextWithoutHeader(
		&settingsText, header.length(), settingsText.length() - header.length());
#else
	QStringView settingTextWithoutHeader =
		QStringView(settingsText).sliced(header.length(), settingsText.length() - header.length());
#endif

	hashCrypt.addData(settingTextWithoutHeader.toLocal8Bit());
	hash = hashCrypt.result();
	// qDebug() << "hash code" << hash.toHex();

	WriteLogString("Settings text prepared", settingsText, 3);

	return size_t(settingsText.size());
}

void cSettings::CreateAnimationString(
	QString &text, const QString &headerText, const std::shared_ptr<cAnimationFrames> frames) const
{
	if (frames)
	{
		if (frames->GetNumberOfFrames() > 0)
		{
			// write section header
			text += "[" + headerText + "]\n";

			// create first row of csv structure (column names)
			QList<cAnimationFrames::sParameterDescription> parameterList =
				frames->GetListOfUsedParameters();
			// header
			text += "frame;";
			text += "framesPerKeyframe;";

			for (int i = 0; i < parameterList.size(); ++i)
			{
				if (parameterList[i].varType == parameterContainer::typeVector3)
				{
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_x;";
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_y;";
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_z";
				}
				else if (parameterList[i].varType == parameterContainer::typeVector4)
				{
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_x;";
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_y;";
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_z;";
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_w";
				}
				else if (parameterList[i].varType == parameterContainer::typeRgb)
				{
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_R;";
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_G;";
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName + "_B";
				}
				else
				{
					text += parameterList[i].containerName + "_" + parameterList[i].parameterName;
				}

				if (i != parameterList.size() - 1)
				{
					text += ";";
				}
			}
			text += "\n";
			for (int f = 0; f < frames->GetNumberOfFrames(); ++f)
			{
				// keyframe index
				text += QString::number(f) + ";";

				// frames per keyframe
				text += QString::number(frames->GetFrame(f).numberOfSubFrames) + ";";

				for (int i = 0; i < parameterList.size(); ++i)
				{
					if (parameterList[i].varType == parameterContainer::typeVector3)
					{
						CVector3 val = frames->GetFrame(f).parameters.Get<CVector3>(
							parameterList[i].containerName + "_" + parameterList[i].parameterName);
						text += QString("%L1").arg(val.x, 0, 'g', 15) + ";";
						text += QString("%L1").arg(val.y, 0, 'g', 15) + ";";
						text += QString("%L1").arg(val.z, 0, 'g', 15);
					}
					else if (parameterList[i].varType == parameterContainer::typeVector4)
					{
						CVector4 val = frames->GetFrame(f).parameters.Get<CVector4>(
							parameterList[i].containerName + "_" + parameterList[i].parameterName);
						text += QString("%L1").arg(val.x, 0, 'g', 15) + ";";
						text += QString("%L1").arg(val.y, 0, 'g', 15) + ";";
						text += QString("%L1").arg(val.z, 0, 'g', 15) + ";";
						text += QString("%L1").arg(val.w, 0, 'g', 15);
					}
					else if (parameterList[i].varType == parameterContainer::typeRgb)
					{
						sRGB val = frames->GetFrame(f).parameters.Get<sRGB>(
							parameterList[i].containerName + "_" + parameterList[i].parameterName);
						text += QString::number(val.R) + ";";
						text += QString::number(val.G) + ";";
						text += QString::number(val.B);
					}
					else
					{
						text += frames->GetFrame(f).parameters.Get<QString>(
							parameterList[i].containerName + "_" + parameterList[i].parameterName);
					}

					if (i != parameterList.size() - 1)
					{
						text += ";";
					}
				}
				text += "\n";
			}
			if (headerText == "keyframes")
			{
				text += "interpolation;";
				for (int i = 0; i < parameterList.size(); ++i)
				{
					switch (parameterList[i].morphType)
					{
						case morphNone: text += "morphNone"; break;
						case morphLinear: text += "morphLinear"; break;
						case morphLinearAngle: text += "morphLinearAngle"; break;
						case morphCatMullRom: text += "morphCatMullRom"; break;
						case morphCatMullRomAngle: text += "morphCatMullRomAngle"; break;
						case morphAkima: text += "morphAkima"; break;
						case morphAkimaAngle: text += "morphAkimaAngle"; break;
						case morphCubic: text += "morphCubic"; break;
						case morphCubicAngle: text += "morphCubicAngle"; break;
						case morphSteffen: text += "morphSteffen"; break;
						case morphSteffenAngle: text += "morphSteffenAngle"; break;
					}
					if (i != parameterList.size() - 1)
					{
						text += ";";
					}
				}
				text += "\n";
			}
		}
	}
}

QString cSettings::CreateHeader() const
{
	QString header("# Mandelbulber settings file\n");
	header += "# version " + QString::number(appVersion, 'f', 2) + "\n";

	switch (format)
	{
		case formatFullText: header += "# all parameters\n"; break;
		case formatCondensedText:
		case formatNetRender: header += "# only modified parameters\n"; break;
		case formatAppSettings: header += "# application settings\n"; break;
	}
	return header;
}

QString cSettings::CreateOneLine(std::shared_ptr<const cParameterContainer> par, QString name) const
{
	QString text;

	cOneParameter parameter = par->GetAsOneParameter(name);

	enumParameterType parType = parameter.GetParameterType();

	bool selNormal =
		(format == formatFullText || format == formatCondensedText) && parType == paramStandard;

	bool selNetRender =
		(format == formatNetRender) && (parType == paramStandard || parType == paramOnlyForNet);

	// adding selected appSettings to standard settings file
	bool additinalAppSetting = false;
	if (parType == paramApp && !listOfAppSettings.isEmpty())
	{
		if (listOfAppSettings.contains(name)) additinalAppSetting = true;
	}
	bool selAppSettings = (format == formatAppSettings && parType == paramApp) || additinalAppSetting;

	if (selNormal || selNetRender || selAppSettings)
	{
		QString value;
		enumVarType type = parameter.GetValueType();
		if (!parameter.isDefaultValue() || format == formatFullText || format == formatNetRender)
		{
			if (type == typeBool)
			{
				bool bValue = parameter.Get<bool>(valueActual);
				value = bValue ? "true" : "false";
			}
			else
			{
				if (parameter.IsEnumeration())
				{
					value = parameter.GetValueByEnumeration();
				}
				else
				{
					value = parameter.Get<QString>(valueActual);
				}
			}

			if (name == "formula_code")
			{
				value = CompressAndCode(value);
			}

			QString script = parameter.GetScript();

			if (script.isEmpty())
			{
				text = QString("%1 %2;\n").arg(name).arg(value);
			}
			else
			{
				text = QString("%1 %2;%3;\n").arg(name).arg(value).arg(script);
			}
		}
	}
	return text;
}

QString cSettings::CompressAndCode(const QString &text) const
{
	QByteArray blob = text.toUtf8();
	QByteArray compressedBlob = qCompress(blob, 9);
	return compressedBlob.toBase64();
}

QString cSettings::DecodeAndDecompress(const QString &text) const
{
	QByteArray compressedBlob = QByteArray::fromBase64(text.toUtf8());
	QByteArray blob = qUncompress(compressedBlob);
	return QString(blob);
}

bool cSettings::SaveToFile(QString filename) const
{
	WriteLogString("Saving settings started", filename, 2);
	QFile qFile(filename);
	if (qFile.open(QIODevice::WriteOnly))
	{
		QTextStream outStream(&qFile);
		outStream << settingsText;
		outStream.flush();
		qFile.close();
		return true;
	}
	else
	{
		cErrorMessage::showMessage(
			QString("Settings file not saved!\n") + filename + "\n" + qFile.errorString(),
			cErrorMessage::errorMessage);
		return false;
	}
}

void cSettings::SaveToClipboard() const
{
	WriteLog("Save settings to clipboard", 2);
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(settingsText);
}

bool cSettings::LoadFromFile(QString filename)
{
	settingsText.clear();
	textPrepared = false;
	WriteLogString("Loading settings started", filename, 2);
	QFile qFile(filename);
	if (qFile.open(QIODevice::ReadOnly))
	{
		QTextStream inStream(&qFile);
		settingsText.append(inStream.readAll());
		qFile.close();
		textPrepared = true;

		// hash code will be needed for generating thumbnails
		QCryptographicHash hashCrypt(QCryptographicHash::Md4);
		hashCrypt.addData(settingsText.toLocal8Bit());
		hash = hashCrypt.result();
		// qDebug() << "hash code" << hash.toHex();

		WriteLogString("Settings loaded", settingsText, 2);

		return true;
	}
	else
	{
		if (!quiet)
		{
			cErrorMessage::showMessage(
				QString("Settings file not loaded!\n") + filename + "\n" + qFile.errorString(),
				cErrorMessage::errorMessage);
		}
		return false;
	}
}

bool cSettings::LoadFromString(const QString &_settingsText)
{
	settingsText = _settingsText;
	textPrepared = true;

	QCryptographicHash hashCrypt(QCryptographicHash::Md4);
	hashCrypt.addData(settingsText.toLocal8Bit());
	hash = hashCrypt.result();
	return true;
}

bool cSettings::LoadFromClipboard()
{
	WriteLog("Load settings from clipboard", 2);
	QClipboard *clipboard = QApplication::clipboard();
	return LoadFromString(clipboard->text());
}

void cSettings::DecodeHeader(QStringList &separatedText)
{
	if (textPrepared)
	{
		try
		{
			int size = separatedText.size();
			if (size < 3) throw QObject::tr("It's not valid Mandelbulber settings file. No header");

			QString firstLine = separatedText[0];

			if (!firstLine.contains("Mandelbulber settings file"))
				throw QObject::tr("It's not valid Mandelbulber settings file. Wrong header");

			QString secondLine = separatedText[1];
			int versionPos = secondLine.lastIndexOf("version");

			if (versionPos < 0)
				throw QObject::tr(
					"It's not valid Mandelbulber settings file. No information about version of file");

			int numberPos = versionPos + QString("version").length();

			bool ok;
			fileVersion = secondLine.mid(numberPos).toDouble(&ok);

			if (!ok)
				throw QObject::tr("It's not valid Mandelbulber settings file. Wrong file version number");

			if (fileVersion > appVersion)
				throw QObject::tr("File was saved in newer version of Mandelbulber\nFile version: ")
					+ QString::number(fileVersion);

			QString thirdLine = separatedText[2];
			if (thirdLine.contains("all parameters"))
			{
				format = formatFullText;
			}
			else if (thirdLine.contains("only modified parameters"))
			{
				format = formatCondensedText;
			}
			else if (thirdLine.contains("application settings"))
			{
				format = formatAppSettings;
			}
			else
			{
				throw QObject::tr(
					"It's not valid Mandelbulber settings file. Format not specified in the header");
			}
		}
		catch (QString &error)
		{
			if (!quiet)
			{
				cErrorMessage::showMessage(error, cErrorMessage::errorMessage);
			}
			textPrepared = false;
			return;
		}
	}
}

void cSettings::Compatibility3(const std::shared_ptr<cKeyframes> &keyframes,
	const std::shared_ptr<cParameterContainer> &par,
	const std::shared_ptr<cFractalContainer> &fractPar)
{
	if (fileVersion < 2.25 && keyframes)
	{
		// conversion of old light rotation parameters to new parameters
		// conversion of old light rotation parameters to new parameters
		QList<cAnimationFrames::sParameterDescription> listOfAnimatedParameters =
			keyframes->GetListOfParameters();
		for (const auto &animatedParameter : listOfAnimatedParameters)
		{
			QString parameterName = animatedParameter.parameterName;

			if (parameterName == "light1_alpha" || parameterName == "light1_beta")
			{
				keyframes->AddAnimatedParameter("main_light1_rotation", par, fractPar);
				int numberOfKeyframes = keyframes->GetNumberOfFrames();
				for (int i = 0; i < numberOfKeyframes; i++)
				{
					cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(i);
					frame.parameters.Set(
						"main_light1_rotation", CVector3(frame.parameters.Get<double>("main_light1_alpha"),
																			frame.parameters.Get<double>("main_light1_beta"), 0.0));
					keyframes->ModifyFrame(i, frame);
				}
				keyframes->RemoveAnimatedParameter("main_light1_alpha");
				keyframes->RemoveAnimatedParameter("main_light1_beta");
				break;
			}
		}
	}
}

bool cSettings::Decode(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> fractPar, std::shared_ptr<cAnimationFrames> frames,
	std::shared_ptr<cKeyframes> keyframes)
{
	WriteLog(
		"cSettings::Decode(std::shared_ptr<cParameterContainer> par, "
		"std::shared_ptr<cFractalContainer> fractPar, cAnimationFrames "
		"*frames)",
		2);

	QString settingsTextTrimmed = settingsText.trimmed();

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
	QStringList separatedText =
		settingsTextTrimmed.split(QRegExp("\n|\r\n|\r"), QString::KeepEmptyParts);
#else
	QStringList separatedText =
		settingsTextTrimmed.split(QRegularExpression("\n|\r\n|\r"), Qt::KeepEmptyParts);
#endif

	DecodeHeader(separatedText);

	int errorCount = 0;
	int csvLine = 0;

	QString section;
	if (textPrepared)
	{
		if (listOfParametersToProcess.isEmpty()) // if not selective load
		{
			// clear settings
			par->ResetAllToDefault();
			if (fractPar)
			{
				for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
					fractPar->at(i)->ResetAllToDefault();
			}
			DeleteAllPrimitiveParams(par);
			listOfLoadedPrimitives.clear();
			DeleteAllMaterialParams(par);
			DeleteAllLightParams(par);

			if (frames)
			{
				frames->ClearAll();
				frames->RemoveAllAudioParameters(par);
			}
			if (keyframes)
			{
				keyframes->ClearAll();
				keyframes->ClearMorphCache();
				keyframes->RemoveAllAudioParameters(par);
			}
		}
		// temporary containers to decode frames
		std::shared_ptr<cParameterContainer> parTemp(new cParameterContainer());
		std::shared_ptr<cFractalContainer> fractTemp(new cFractalContainer());

		for (int l = 3; l < separatedText.size(); l++)
		{
			QString line = separatedText[l];
			bool isNewSection = CheckSection(line, section);

			if (isNewSection)
			{
				csvLine = 0;
				continue;
			}
			else if (section == QString("description"))
			{
				// concat multi-line description
				QString description = "";
				if (par->IfExists("description")) description = par->Get<QString>("description");
				if (description != "") description += "\n";
				description += line;
				par->Set("description", description);
				continue;
			}
			else
			{
				if (line == "") continue;
				bool result = false;
				if (section == QString("main_parameters"))
				{
					if (!listOfParametersToProcess.isEmpty()) // selective loading
					{

						int firstSpace = line.indexOf(' ');
						QString parameterName = line.left(firstSpace);

						if (forcedFractalFormulaIndex > 0)
						{
							// replace index in parameter name
							bool conversionOK = false;
							parameterName.right(1).toInt(&conversionOK);
							if (conversionOK) // if last letter is a number, then replace index in settings line
							{
								QString digit = QString::number(forcedFractalFormulaIndex);
								line[firstSpace - 1] = digit[0];
								parameterName[firstSpace - 1] = digit[0];
							}
						}

						if (!listOfParametersToProcess.contains(QString("main_") + parameterName)) continue;
					}

					result = DecodeOneLine(par, line);
				}
				else if (section.contains("fractal"))
				{
					int i = section.right(1).toInt() - 1;
					if (forcedFractalFormulaIndex > 0) i = forcedFractalFormulaIndex - 1;

					if (!listOfParametersToProcess.isEmpty()) // selective loading
					{
						int firstSpace = line.indexOf(' ');
						QString parameterName = line.left(firstSpace);
						if (forcedFractalFormulaIndex == -1
								&& !listOfParametersToProcess.contains(
									QString("fractal%1_").arg(i) + parameterName))
							continue;
					}

					if (fractPar) result = DecodeOneLine(fractPar->at(i), line);
				}
				else if (section == QString("frames"))
				{
					if (listOfParametersToProcess.isEmpty())
					{
						if (frames)
						{
							if (csvLine == 0)
							{
								CheckIfMaterialsAreDefined(par);
								*parTemp = *par;
								if (fractPar) *fractTemp = *fractPar;

								result = DecodeFramesHeader(line, par, fractPar, frames);
								csvLine++;
							}
							else
							{
								result = DecodeFramesLine(line, parTemp, fractTemp, frames);
								csvLine++;
							}
						}
						else
						{
							result = true;
						}
					}
					else // if listOfParametersToProcess is not empty
					{
						result = true;
					}
				}
				else if (section == QString("keyframes"))
				{
					if (listOfParametersToProcess.isEmpty())
					{
						if (keyframes)
						{
							if (csvLine == 0)
							{
								CheckIfMaterialsAreDefined(par);
								*parTemp = *par;
								if (fractPar) *fractTemp = *fractPar;

								result = DecodeFramesHeader(line, par, fractPar, keyframes);
								csvLine++;
							}
							else
							{
								result = DecodeFramesLine(line, parTemp, fractTemp, keyframes);
								csvLine++;
							}
						}
						else
						{
							result = true;
						}
					}
					else // if listOfParametersToProcess is not empty
					{
						result = true;
					}
				}

				if (!result)
				{
					if (!quiet)
					{
						QString errorMessage = QObject::tr("Error in settings file. Line: ")
																	 + QString::number(l) + " (" + line + ")";
						cErrorMessage::showMessage(errorMessage, cErrorMessage::errorMessage);
					}
					errorCount++;
					if (errorCount > 3)
					{
						if (!quiet)
						{
							cErrorMessage::showMessage(
								QObject::tr("Too many errors in settings file"), cErrorMessage::errorMessage);
						}
						return false;
					}
				}
			}
		}

		// add default parameters for animation
		if (keyframes && listOfParametersToProcess.isEmpty())
		{
			if (keyframes->GetListOfUsedParameters().size() == 0)
			{
				keyframes->AddAnimatedParameter("camera", par->GetAsOneParameter("camera"), par);
				keyframes->AddAnimatedParameter("target", par->GetAsOneParameter("target"), par);
				keyframes->AddAnimatedParameter("camera_top", par->GetAsOneParameter("camera_top"), par);
			}
		}

		// check if there is at least one material defined
		if (format != formatAppSettings)
		{
			CheckIfMaterialsAreDefined(par);
		}

		if (listOfParametersToProcess.isEmpty())
		{
			// now when anim sound parameters are already prepared by animation, all animsound parameters
			// can be processed
			if (keyframes && linesWithSoundParameters.length() > 0)
			{
				foundAnimSoundParameters = true;
				for (const auto &linesWithSoundParameter : linesWithSoundParameters)
				{
					bool result;
					result = DecodeOneLine(par, linesWithSoundParameter);

					if (!result)
					{
						if (!quiet)
						{
							QString errorMessage =
								QObject::tr("Error in settings file. Line: ") + linesWithSoundParameter;
							cErrorMessage::showMessage(errorMessage, cErrorMessage::errorMessage);
						}
						errorCount++;
						if (errorCount > 3)
						{
							if (!quiet)
							{
								cErrorMessage::showMessage(
									QObject::tr("Too many errors in settings file"), cErrorMessage::errorMessage);
							}
							return false;
						}
					}
				}
			}
		}

		if (format != formatAppSettings)
		{
			Compatibility2(par, fractPar);

			Compatibility3(keyframes, par, fractPar);
		}

		if (listOfParametersToProcess.isEmpty())
		{
			if (frames) frames->LoadAllAudioFiles(par);
			if (keyframes) keyframes->LoadAllAudioFiles(par);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool cSettings::CheckIfMaterialsAreDefined(std::shared_ptr<cParameterContainer> par)
{
	bool matParameterFound = false;
	QList<QString> list = par->GetListOfParameters();
	for (auto parameterName : list)
	{
		if (parameterName.left(3) == "mat")
		{
			matParameterFound = true;
			break;
		}
	}

	if (par->IfExists("formula_material_id")) // if there is checked full set of parameters
	{
		if (par->Get<int>("formula_material_id") == 1
				&& !par->IfExists("mat1_is_defined")) // if material 1 is used but still not defined
		{
			matParameterFound = false;
		}
	}

	if (!matParameterFound)
	{
		InitMaterialParams(1, par);
		par->Set("mat1_is_defined", true);
	}
	return matParameterFound;
}

bool cSettings::DecodeOneLine(std::shared_ptr<cParameterContainer> par, QString line)
{
	int firstSpace = line.indexOf(' ');
	int semicolon = line.indexOf(';');
	QString parameterName = line.left(firstSpace);
	QString value = line.mid(firstSpace + 1, semicolon - firstSpace - 1);
	QString script;

	// lokking for script
	if (semicolon < line.length() - 2)
	{
		int semicolon2 = line.indexOf(';', semicolon + 1);
		script = line.mid(semicolon + 1, semicolon2 - semicolon - 1);
	}

	Compatibility(parameterName, value);

	// skip processing of parameter in case of compatibility issue
	if (parameterName == "skip") return true;

	if (parameterName.left(parameterName.indexOf('_')) == "primitive")
	{
		if (!par->IfExists(parameterName))
		{
			QStringList split = parameterName.split('_');
			QString primitiveName = split.at(0) + "_" + split.at(1) + "_" + split.at(2);
			int primitiveIndex = split.at(2).toInt();
			fractal::enumObjectType objectType = cPrimitives::PrimitiveNameToEnum(split.at(1));
			sPrimitiveItem item(objectType, primitiveIndex, primitiveName, split.at(1));
			InitPrimitiveParams(item, par);
			listOfLoadedPrimitives.append(primitiveName);
		}
	}

	if (parameterName.left(3) == "mat")
	{
		if (!par->IfExists(parameterName))
		{
			int positionOfDash = parameterName.indexOf('_');
			int matIndex = parameterName.mid(3, positionOfDash - 3).toInt();
			QString shortName = parameterName.mid(positionOfDash + 1);
			if (cMaterial::paramsList.indexOf(shortName) >= 0)
			{
				InitMaterialParams(matIndex, par);
				PreCompatibilityMaterials(matIndex, par);
				par->Set(QString("mat%1_is_defined").arg(matIndex), true);
			}
			else
			{
				if (!quiet)
				{
					cErrorMessage::showMessage(
						QObject::tr("Unknown parameter: ") + parameterName, cErrorMessage::errorMessage);
				}
				return false;
			}
		}
	}

	if (parameterName.left(5) == "light")
	{
		if (!par->IfExists(parameterName))
		{
			int positionOfDash = parameterName.indexOf('_');
			int lightIndex = parameterName.mid(5, positionOfDash - 5).toInt();
			QString shortName = parameterName.mid(positionOfDash + 1);
			if (cLight::paramsList.indexOf(shortName) >= 0)
			{
				InitLightParams(lightIndex, par);
				par->Set(QString("light%1_is_defined").arg(lightIndex), true);
			}
			else
			{
				if (!quiet)
				{
					cErrorMessage::showMessage(
						QObject::tr("Unknown parameter: ") + parameterName, cErrorMessage::errorMessage);
				}
				return false;
			}
		}
	}

	if (parameterName.left(9) == "animsound")
	{
		if (!foundAnimSoundParameters)
		{
			linesWithSoundParameters.append(
				line);		 // added line for further processing (after animation is loaded)
			return true; // parameter will be processed later
		}
	}

	if (parameterName.left(15) == "flightanimsound")
	{
		// ignore this line from settings
		return true;
	}

	enumVarType varType = par->GetVarType(parameterName);

	if (varType == typeNull)
	{
		if (!quiet)
		{
			cErrorMessage::showMessage(
				QObject::tr("Unknown parameter: ") + parameterName, cErrorMessage::errorMessage);
		}
		return false;
	}
	else
	{
		if (varType != typeString)
		{
			if (value.size() == 0)
			{
				if (!quiet)
				{
					cErrorMessage::showMessage(
						QObject::tr("Missing value for parameter %1").arg(parameterName),
						cErrorMessage::errorMessage);
				}
				return false;
			}
		}
		if (varType == typeBool)
		{
			value = value == QString("true") ? "1" : "0";
		}
		else if (varType == typeDouble || varType == typeVector3 || varType == typeVector4)
		{
			value = everyLocaleDouble(value);
		}

		if (parameterName == "formula_code")
		{
			value = DecodeAndDecompress(value);
		}

		if (par->GetAsOneParameter(parameterName).IsEnumeration())
		{
			par->Set(parameterName, par->GetAsOneParameter(parameterName).GetIndexByEnumeration(value));
		}
		else
		{
			par->Set(parameterName, value);
		}

		if (!script.isEmpty()) par->SetScript(parameterName, script);

		return true;
	}
}

bool cSettings::CheckSection(QString text, QString &section)
{
	if (text.left(1) == "[" && text.right(1) == "]")
	{
		section = text.mid(1, text.length() - 2);
		return true;
	}
	return false;
}

void cSettings::Compatibility(QString &name, QString &value) const
{
	if (fileVersion <= 2.01)
	{
		if (name.indexOf("aux_light_predefined") >= 0)
		{
			name.replace("aux_light_predefined", "aux_light");
		}

		if (name == QString("volumetric_light_intensity_0"))
		{
			name = QString("main_light_volumetric_intensity");
		}
		else if (name == QString("volumetric_light_enabled_0"))
		{
			name = QString("main_light_volumetric_enabled");
		}
		else if (name.indexOf("volumetric_light") >= 0)
		{
			name.replace("volumetric_light", "aux_light_volumetric");
		}
	}

	if (fileVersion <= 2.04)
	{
		if (name == QString("fractal_constant_factor"))
		{
			QString newValue = value + " " + value + " " + value;
			value = newValue;
		}
	}

	if (fileVersion <= 2.06)
	{
		if (name == QString("linear_DE_mode"))
		{
			name = QString("delta_DE_function");
			value = QString("1");
		}
	}

	if (fileVersion <= 2.071)
	{
		if (name == QString("shading")) name.replace("shading", "mat1_shading");

		if (name == QString("specular")) name.replace("specular", "mat1_specular");

		if (name == QString("reflect")) name.replace("reflect", "mat1_reflectance");

		if (name == QString("transparency_of_surface"))
			name.replace("transparency_of_surface", "mat1_transparency_of_surface");

		if (name == QString("transparency_of_interior"))
			name.replace("transparency_of_interior", "mat1_transparency_of_interior");

		if (name == QString("transparency_index_of_refraction"))
			name.replace("transparency_index_of_refraction", "mat1_transparency_index_of_refraction");

		if (name == QString("transparency_interior_color"))
			name.replace("transparency_interior_color", "mat1_transparency_interior_color");

		if (name == QString("fresnel_reflectance"))
			name.replace("fresnel_reflectance", "mat1_fresnel_reflectance");

		if (name == QString("coloring_random_seed"))
			name.replace("coloring_random_seed", "mat1_coloring_random_seed");

		if (name == QString("coloring_saturation"))
			name.replace("coloring_saturation", "mat1_coloring_saturation");

		if (name == QString("coloring_speed")) name.replace("coloring_speed", "mat1_coloring_speed");

		if (name == QString("coloring_palette_size"))
			name.replace("coloring_palette_size", "mat1_coloring_palette_size");

		if (name == QString("coloring_palette_offset"))
			name.replace("coloring_palette_offset", "mat1_coloring_palette_offset");

		if (name == QString("fractal_color"))
			name.replace("fractal_color", "mat1_use_colors_from_palette");

		if (name == QString("surface_color_palette"))
			name.replace("surface_color_palette", "mat1_surface_color_palette");

		if (name == QString("fractal_coloring_algorithm"))
			name.replace("fractal_coloring_algorithm", "mat1_fractal_coloring_algorithm");

		if (name == QString("fractal_coloring_sphere_radius"))
			name.replace("fractal_coloring_sphere_radius", "mat1_fractal_coloring_sphere_radius");

		if (name == QString("fractal_coloring_line_direction"))
			name.replace("fractal_coloring_line_direction", "mat1_fractal_coloring_line_direction");
	}
	if (fileVersion < 2.09)
	{
		if (name == QString("delta_DE_function"))
		{
			if (value == QString("0"))
			{
				value = "2";
			}
			else if (value == QString("2"))
			{
				value = "0";
			}
		}
	}

	if (fileVersion < 2.12)
	{
		name.replace("gpu_", "opencl_");
	}

	if (fileVersion < 2.13)
	{
		if (name.contains("primitive_water"))
		{
			name.replace("amplitude", "relative_amplitude");
		}
	}

	if (fileVersion < 2.19)
	{
		if (name.contains("surface_color_palette"))
		{
			name.replace("surface_color_palette", "surface_color_gradient");

			QStringList split = value.split(" ");
			int numberOfColors = split.size();

			// in case if last color is not a valid color (e.g. empty)
			if (split.last().size() < 6) numberOfColors -= 1;

			double step = 1.0 / numberOfColors;
			QString newValue;
			for (int i = 0; i < numberOfColors; i++)
			{
				int pos = int(i * step * 10000.0);
				if (i > 0) newValue += " ";
				newValue += QString("%1 %2").arg(pos).arg(split[i]);
			}
			value = newValue;
		}

		if (name.contains("luminosity_color_thesame"))
			name.replace("luminosity_color_thesame", "luminosity_gradient_enable");

		if (name.contains("reflections_color_thesame"))
			name.replace("reflections_color_thesame", "reflectance_gradient_enable");

		if (name.contains("transparency_color_thesame"))
			name.replace("transparency_color_thesame", "transparency_gradient_enable");

		if (name.contains("coloring_palette_size")) name = "skip";

		if (name.contains("coloring_random_seed")) name = "skip";

		if (name.contains("coloring_saturation")) name = "skip";
	}

	if (fileVersion < 2.25)
	{
		// conversion of main light to light #1

		if (name.contains("main_light_intensity"))
			name.replace("main_light_intensity", "light1_intensity");

		if (name == QString("main_light_visibility"))
			name.replace("main_light_visibility", "light1_visibility");

		if (name.contains("main_light_visibility_size"))
			name.replace("main_light_visibility_size", "light1_size");

		if (name.contains("main_light_contour_sharpness"))
			name.replace("main_light_contour_sharpness", "light1_contour_sharpness");

		if (name.contains("main_light_alpha")) name.replace("main_light_alpha", "light1_alpha");

		if (name.contains("main_light_beta")) name.replace("main_light_beta", "light1_beta");

		if (name.contains("main_light_colour")) name.replace("main_light_colour", "light1_color");

		if (name.contains("penetrating_lights"))
			name.replace("penetrating_lights", "light1_penetrating");

		if (name.contains("shadows_enabled")) name.replace("shadows_enabled", "light1_cast_shadows");

		if (name.contains("shadows_cone_angle"))
			name.replace("shadows_cone_angle", "light1_soft_shadow_cone");

		if (name.contains("main_light_enable")) name.replace("main_light_enable", "light1_enabled");

		if (name.contains("main_light_position_relative"))
			name.replace("main_light_position_relative", "light1_relative_position");

		if (name.contains("main_light_volumetric_intensity"))
			name.replace("main_light_volumetric_intensity", "light1_volumetric_visibility");

		if (name.contains("main_light_volumetric_enabled"))
			name.replace("main_light_volumetric_enabled", "light1_volumetric");

		if (name.contains("aux_light"))
		{
			// TODO: copy size and visibility to all another lights

			if (name.contains("visibility_size"))
				name.replace("aux_light_visibility_size", "light2_size");

			else if (name.contains("visibility"))
				name.replace("aux_light_visibility", "light2_visibility");

			else if (name.contains("aux_light_place_behind"))
			{
			} // do nothing

			else
			{
				QStringList split = name.split("_");
				int lightIndex = split.last().toInt();
				QString prefix =
					QString("light%1").arg(lightIndex + 1); // aux lights will start from index = 2

				if (split.at(2) == "intensity")
					name = QString("%1_intensity").arg(prefix);
				else if (split.at(2) == "position")
					name = QString("%1_position").arg(prefix);
				if (split.at(2) == "position")
					name = QString("%1_position").arg(prefix);
				else if (split.at(2) == "intensity")
					name = QString("%1_intensity").arg(prefix);
				else if (split.at(2) == "enabled")
					name = QString("%1_enabled").arg(prefix);
				else if (split.at(2) == "colour")
					name = QString("%1_color").arg(prefix);
				else if (split.at(2) == "volumetric")
				{
					if (split.at(3) == "intensity")
						name = QString("%1_volumetric_visibility").arg(prefix);
					else if (split.at(3) == "enabled")
						name = QString("%1_volumetric").arg(prefix);
				}
			}
		}
	}

	if (fileVersion < 2.28)
	{
		if (name == QString("random_lights_one_color_enable"))
		{
			name = QString("random_lights_coloring_type");
			if (value.contains("true"))
			{
				value = QString("single");
			}
		}
	}
}

void cSettings::Compatibility2(
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fract)
{
	if (fileVersion <= 2.06)
	{
		if (fractal::enumDEFunctionType(par->Get<int>("delta_DE_function"))
				!= fractal::linearDEFunction)
			par->Set("delta_DE_function", int(fractal::logarithmicDEFunction));

		if (fract)
		{
			for (int i = 0; i < 4; i++)
			{
				fract->at(i)->Set("IFS_rotation_enabled", true);
			}
		}
	}

	if (fileVersion <= 2.071)
	{
		for (int i = 0; i < listOfLoadedPrimitives.size(); i++)
		{
			int materialId = i + 2;
			InitMaterialParams(materialId, par);
			par->Set(cMaterial::Name("is_defined", materialId), true);
			par->Set(cMaterial::Name("name", materialId), listOfLoadedPrimitives[i]);
			par->Set(cMaterial::Name("surface_color", materialId),
				par->Get<sRGB>(listOfLoadedPrimitives[i] + "_color"));
			par->Set(cMaterial::Name("reflectance", materialId),
				par->Get<double>(listOfLoadedPrimitives[i] + "_reflection"));
			par->Set(cMaterial::Name("use_colors_from_palette", materialId), false);
			par->Set(cMaterial::Name("fresnel_reflectance", materialId),
				par->Get<bool>("mat1_fresnel_reflectance"));
			par->Set(cMaterial::Name("transparency_index_of_refraction", materialId),
				par->Get<double>("mat1_transparency_index_of_refraction"));
			par->Set(cMaterial::Name("transparency_of_surface", materialId),
				par->Get<double>("mat1_transparency_of_surface"));
			par->Set(cMaterial::Name("transparency_of_interior", materialId),
				par->Get<double>("mat1_transparency_of_interior"));
			par->Set(cMaterial::Name("transparency_interior_color", materialId),
				par->Get<double>("mat1_transparency_interior_color"));
			par->Set(cMaterial::Name("specular", materialId), par->Get<double>("mat1_specular"));
			par->Set(cMaterial::Name("shading", materialId), par->Get<double>("mat1_shading"));
			par->Set(listOfLoadedPrimitives[i] + "_material_id", materialId);
		}
	}

	if (fileVersion <= 2.12)
	{
		if (par->Get<bool>("iteration_fog_enable"))
		{
			par->Set("iteration_fog_brightness_boost", 100.0);
		}
		for (int i = 0; i < listOfLoadedPrimitives.size(); i++)
		{
			if (listOfLoadedPrimitives[i].contains("primitive_water"))
			{
				double relativeAmplitude =
					par->Get<double>(listOfLoadedPrimitives[i] + "_relative_amplitude")
					/ par->Get<double>(listOfLoadedPrimitives[i] + "_length");
				par->Set(listOfLoadedPrimitives[i] + "_relative_amplitude", relativeAmplitude);
			}
		}
	}

	if (fileVersion < 2.19)
	{
		QSet<QString> materialList;
		QStringList listOfParameters = par->GetListOfParameters();
		for (QString paramName : listOfParameters)
		{
			if (paramName.left(3) == "mat" && paramName.contains("is_defined"))
			{
				int firstDash = paramName.indexOf("_");
				materialList.insert(paramName.left(firstDash));
			}
		}

		for (QString mat : materialList)
		{
			QString coloringOffsetParameter = mat + "_coloring_palette_offset";
			QString colorPaletteParameter = mat + "_surface_color_gradient";
			QString coloringSpeedParameter = mat + "_coloring_speed";
			int paletteSize = par->Get<QString>(colorPaletteParameter).split(" ").size() / 2;
			par->Set(coloringOffsetParameter, par->Get<double>(coloringOffsetParameter) / paletteSize);
			par->Set(
				coloringSpeedParameter, par->Get<double>(coloringSpeedParameter) * 10.0 / paletteSize);

			if (par->Get<bool>(mat + "_luminosity_gradient_enable"))
				par->Set(mat + "_luminosity_gradient", par->Get<QString>(mat + "_surface_color_gradient"));

			if (par->Get<bool>(mat + "_reflectance_gradient_enable"))
				par->Set(mat + "_reflectance_gradient", par->Get<QString>(mat + "_surface_color_gradient"));

			if (par->Get<bool>(mat + "_transparency_gradient_enable"))
				par->Set(
					mat + "_transparency_gradient", par->Get<QString>(mat + "_surface_color_gradient"));
		}
	}

	if (fileVersion < 2.20)
	{
		if (par->IfExists("delta_DE_method"))
		{
			if (fractal::enumDEMethod(par->Get<int>("delta_DE_method")) == fractal::preferredDEMethod)
			{
				par->Set("delta_DE_function", int(fractal::preferredDEFunction));
			}
		}
	}

	if (fileVersion < 2.21)
	{
		params::enumPerspectiveType perspectiveType =
			params::enumPerspectiveType(par->Get<int>("perspective_type"));
		double fov = par->Get<double>("fov");
		if (fov == 53.13) fov = 1.0;

		double fovDegrees = 0.0;
		switch (perspectiveType)
		{
			case params::perspThreePoint:
			{
				fovDegrees = atan(fov / 2.0) * 360.0 / M_PI;
				break;
			}
			case params::perspFishEye:
			case params::perspFishEyeCut:
			{
				fovDegrees = fov * 180.0;
				break;
			}
			case params::perspEquirectangular:
			{
				fovDegrees = fov * 360.0;
				break;
			}
		}
		par->Set("fov", fovDegrees);
	}

	if (fileVersion < 2.22)
	{
		if (fract)
		{
			for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
			{
				if (fract->at(i)->Get<CVector3>("IFS_edge").Length() > 0)
				{
					fract->at(i)->Set("IFS_edge_enabled", true);
				}
			}
		}
	}

	if (fileVersion < 2.25)
	{
		// FIXME: correct default lights parameters of aux lights

		if (par->IfExists("light1_is_defined"))
		{
			par->Set("light1_rotation",
				CVector3(par->Get<double>("light1_alpha"), par->Get<double>("light1_beta"), 0.0));
		}
		else
		{
			InitLightParams(1, par);
			par->Set("light1_enabled", true);
			par->Set("light1_is_defined", true);
		}

		// copy light visibility and intensity to all another lights
		if (par->IfExists("light2_is_defined"))
		{
			for (int i = 3; i <= 5; i++)
			{
				if (par->IfExists(QString("light%1_is_defined").arg(i)))
				{
					par->Set(QString("light%1_visibility").arg(i), par->Get<double>("light2_visibility"));
					par->Set(QString("light%1_size").arg(i), par->Get<double>("light2_size"));
				}
			}
		}

		// correct intensity of lights
		for (int i = 2; i <= 5; i++)
		{
			if (par->IfExists(QString("light%1_is_defined").arg(i)))
			{
				if (!par->isDefaultValue(QString("light%1_intensity").arg(i)))
				{
					par->Set(QString("light%1_intensity").arg(i),
						par->Get<double>(QString("light%1_intensity").arg(i)) / 4.0);
				}

				if (!par->isDefaultValue(QString("light%1_size").arg(i)))
				{
					par->Set(
						QString("light%1_size").arg(i), par->Get<double>(QString("light%1_size").arg(i)) * 2.0);
				}

				if (par->Get<double>(QString("light%1_volumetric").arg(i)))
				{
					par->Set(QString("light%1_volumetric_visibility").arg(i),
						par->Get<double>(QString("light%1_volumetric_visibility").arg(i))
							/ par->Get<double>(QString("light%1_intensity").arg(i)));
				}

				par->Set(QString("light%1_penetrating").arg(i), par->Get<bool>("light1_penetrating"));
				par->Set(QString("light%1_cast_shadows").arg(i), par->Get<bool>("light1_cast_shadows"));
			}
		}

		// correct intensity of random lights
		if (par->Get<bool>("random_lights_group"))
		{
			int numberOfRandomLights = par->Get<int>("random_lights_number");
			par->Set("random_lights_intensity",
				par->Get<double>("random_lights_intensity") / (numberOfRandomLights / 4 + 4));
			par->Set(
				"random_lights_size", par->Get<double>("light2_size") * (sqrt(numberOfRandomLights) / 4));
		}
	}

	if (fileVersion < 2.28)
	{
		if (par->IfExists(cLight::Name("is_defined", 1)))
		{
			if (par->Get<bool>("iteration_fog_enable"))
			{
				double iterFogBoost = par->Get<double>("iteration_fog_brightness_boost");
				par->Set(cLight::Name("intensity", 1),
					par->Get<double>(cLight::Name("intensity", 1)) / iterFogBoost);
			}
		}
	}

	if (fileVersion < 2.29)
	{
		if (par->Get<bool>("boolean_operators"))
		{
			int maxiter = par->Get<int>("N");
			for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
			{
				par->Set("formula_maxiter", i, maxiter);
			}
		}
	}
}

bool cSettings::DecodeFramesHeader(QString line, std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> fractPar, std::shared_ptr<cAnimationFrames> frames)
{
	QStringList lineSplit = line.split(';');
	try
	{
		if (lineSplit.size() > 0)
		{
			if (lineSplit[0] != "frame")
			{
				throw QObject::tr("Missing column 'frame' in the list of animation frames");
			}

			int firstParameterColumn = 1;
			if (fileVersion >= 2.26)
			{
				if (lineSplit[1] != "framesPerKeyframe")
				{
					throw QObject::tr("Missing column 'framesPerKeyframe' in the list of animation frames");
				}
				firstParameterColumn = 2;
			}

			for (int i = firstParameterColumn; i < lineSplit.size(); ++i)
			{
				QString fullParameterName = lineSplit[i];
				if (fullParameterName.length() > 2)
				{
					QString lastTwo = fullParameterName.right(2);
					if (lastTwo == "_x") // check if it's CVector4
					{
						// check if parameter with _x doesn't exists in the container
						int firstUnderscore = fullParameterName.indexOf('_');
						QString containerName = fullParameterName.left(firstUnderscore);
						QString parameterName = fullParameterName.mid(firstUnderscore + 1);
						std::shared_ptr<cParameterContainer> selectedContainer =
							cAnimationFrames::ContainerSelector(containerName, par, fractPar);

						if (!selectedContainer->IfExists(parameterName))
						{
							// check if there are at least 2 parameters left and they are *_y and *_z
							bool isCVector4 = false;
							if (i + 3 < lineSplit.size())
							{
								QString lastTwoY = lineSplit[i + 1].right(2);
								QString lastTwoZ = lineSplit[i + 2].right(2);
								QString lastTwoW = lineSplit[i + 3].right(2);
								if (lastTwoY == "_y" && lastTwoZ == "_z" && lastTwoW == "_w")
								{
									fullParameterName = fullParameterName.left(fullParameterName.length() - 2);
									i += 3;
									isCVector4 = true;
								}
							}

							if (!isCVector4 && i + 2 < lineSplit.size()) // check if it's CVector3
							{
								QString lastTwoY = lineSplit[i + 1].right(2);
								QString lastTwoZ = lineSplit[i + 2].right(2);
								if (lastTwoY == "_y" && lastTwoZ == "_z")
								{
									fullParameterName = fullParameterName.left(fullParameterName.length() - 2);
									i += 2;
								}
							}
						}
					}
					else if (lastTwo == "_R") // check if it's RGB
					{
						// check if parameter with _R doesn't exists in the container
						int firstUnderscore = fullParameterName.indexOf('_');
						QString containerName = fullParameterName.left(firstUnderscore);
						QString parameterName = fullParameterName.mid(firstUnderscore + 1);
						std::shared_ptr<cParameterContainer> selectedContainer =
							cAnimationFrames::ContainerSelector(containerName, par, fractPar);

						if (!selectedContainer->IfExists(parameterName))
						{
							// check if there are at least 2 parameters left and they are *_G and *_B
							if (i + 2 < lineSplit.size())
							{
								QString lastTwoG = lineSplit[i + 1].right(2);
								QString lastTwoB = lineSplit[i + 2].right(2);
								if (lastTwoG == "_G" && lastTwoB == "_B")
								{
									fullParameterName = fullParameterName.left(fullParameterName.length() - 2);
									i += 2;
								}
							}
						}
					}
				}

				// compatibility with older settings
				int firstUnderscore = fullParameterName.indexOf('_');
				QString containerName = fullParameterName.left(firstUnderscore);
				QString parameterName = fullParameterName.mid(firstUnderscore + 1);
				QString value = "";
				Compatibility(parameterName, value);
				// reconstruction of full parameter name
				fullParameterName = containerName + "_" + parameterName;

				bool result = frames->AddAnimatedParameter(fullParameterName, par, fractPar);
				if (!result)
				{
					throw QObject::tr("Unknown parameter in animation frames: ") + fullParameterName;
				}
			}
		}
		else
		{
			throw QObject::tr("No valid list of parameters for animation frames");
		}
	}
	catch (QString &error)
	{
		cErrorMessage::showMessage(error, cErrorMessage::errorMessage);
		return false;
	}

	csvNoOfColumns = lineSplit.size();
	return true;
}

bool cSettings::DecodeFramesLine(QString line, std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> fractPar, std::shared_ptr<cAnimationFrames> frames)
{
	QStringList lineSplit = line.split(';');
	QList<cAnimationFrames::sParameterDescription> parameterList = frames->GetListOfUsedParameters();
	int column = 0;
	int numberOfSubFrames = 0;

	try
	{
		if (lineSplit.size() > 0 && lineSplit[0] == QString("interpolation"))
		{
			// interpolation
			if (lineSplit.size() - 1 == parameterList.size())
			{
				for (int i = 0; i < parameterList.size(); i++)
				{
					column++;
					enumMorphType morphType = morphNone;
					if (lineSplit[column] == "morphLinear")
						morphType = morphLinear;
					else if (lineSplit[column] == "morphLinearAngle")
						morphType = morphLinearAngle;
					else if (lineSplit[column] == "morphCatMullRom")
						morphType = morphCatMullRom;
					else if (lineSplit[column] == "morphCatMullRomAngle")
						morphType = morphCatMullRomAngle;
					else if (lineSplit[column] == "morphAkima")
						morphType = morphAkima;
					else if (lineSplit[column] == "morphAkimaAngle")
						morphType = morphAkimaAngle;
					else if (lineSplit[column] == "morphCubic")
						morphType = morphCubic;
					else if (lineSplit[column] == "morphCubicAngle")
						morphType = morphCubicAngle;
					else if (lineSplit[column] == "morphSteffen")
						morphType = morphSteffen;
					else if (lineSplit[column] == "morphSteffenAngle")
						morphType = morphSteffenAngle;
					static_pointer_cast<cKeyframes>(frames)->ChangeMorphType(i, morphType);
				}
				return true;
			}
			else
			{
				throw QObject::tr("Wrong number of interpolation columns");
			}
		}
		else if (lineSplit.size() == csvNoOfColumns)
		{
			int frameCount = lineSplit[0].toInt();

			if (fileVersion >= 2.26)
			{
				numberOfSubFrames = lineSplit[1].toInt();
				column++;
			}
			else
			{
				numberOfSubFrames = par->Get<int>("frames_per_keyframe");
			}

			if (frameCount == frames->GetNumberOfFrames())
			{
				column++;
				for (auto &parameterDescription : parameterList)
				{
					using namespace parameterContainer;
					enumVarType type = parameterDescription.varType;
					QString containerName = parameterDescription.containerName;
					QString parameterName = parameterDescription.parameterName;
					std::shared_ptr<cParameterContainer> container =
						frames->ContainerSelector(containerName, par, fractPar);

					if (type == typeVector3)
					{
						CVector3 vect;
						vect.x = systemData.locale.toDouble(everyLocaleDouble(lineSplit[column]));
						vect.y = systemData.locale.toDouble(everyLocaleDouble(lineSplit[column + 1]));
						vect.z = systemData.locale.toDouble(everyLocaleDouble(lineSplit[column + 2]));
						column += 2;
						container->Set(parameterName, vect);
					}
					else if (type == typeVector4)
					{
						CVector4 vect;
						vect.x = systemData.locale.toDouble(everyLocaleDouble(lineSplit[column]));
						vect.y = systemData.locale.toDouble(everyLocaleDouble(lineSplit[column + 1]));
						vect.z = systemData.locale.toDouble(everyLocaleDouble(lineSplit[column + 2]));
						vect.w = systemData.locale.toDouble(everyLocaleDouble(lineSplit[column + 3]));
						column += 3;
						container->Set(parameterName, vect);
					}
					else if (type == typeRgb)
					{
						sRGB vect;
						vect.R = lineSplit[column].toInt();
						vect.G = lineSplit[column + 1].toInt();
						vect.B = lineSplit[column + 2].toInt();
						column += 2;
						container->Set(parameterName, vect);
					}
					else
					{
						QString val;
						if (type == typeDouble)
						{
							val = everyLocaleDouble(lineSplit[column]);
						}
						else
						{
							val = lineSplit[column];
						}
						container->Set(parameterName, val);
					}
					column++;
				}
			}
			else
			{
				throw QObject::tr("Missing frame no ") + QString::number(frames->GetNumberOfFrames());
			}
		}
		else
		{
			throw QObject::tr("Wrong number of columns");
		}
	}
	catch (QString &error)
	{
		cErrorMessage::showMessage(error, cErrorMessage::errorMessage);
		return false;
	}

	Compatibility2(par, fractPar);
	frames->AddFrame(par, fractPar, numberOfSubFrames);

	return true;
}

QString cSettings::GetSettingsText() const
{
	if (textPrepared)
	{
		return settingsText;
	}
	else
	{
		return QString();
	}
}

QString cSettings::everyLocaleDouble(QString txt)
{
	QString txtOut;
	if (systemData.decimalPoint == ',') txtOut = txt.replace('.', ',');
	if (systemData.decimalPoint == '.') txtOut = txt.replace(',', '.');
	return txtOut;
}

void cSettings::PreCompatibilityMaterials(int matIndex, std::shared_ptr<cParameterContainer> par)
{
	if (fileVersion < 2.15)
	{
		par->Set(cMaterial::Name("metallic", matIndex), false);
		par->Set(cMaterial::Name("specular", matIndex), 1.0);
		par->Set(cMaterial::Name("specular_width", matIndex), 1.0);
	}
}
