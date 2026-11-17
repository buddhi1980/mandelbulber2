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

#include <algorithm>

#include <QClipboard>
#include <QCryptographicHash>
#include <QSet>

#include "animation_frames.hpp"
#include "error_message.hpp"
#include "fractal_container.hpp"
#include "fractal_enums.h"
#include "initparameters.hpp"
#include "keyframes.hpp"
#include "light.h"
#include "lights.hpp"
#include "material.h"
#include "object_node_type.h"
#include "primitives.h"
#include "projection_3d.hpp"
#include "system.hpp"
#include "system_data.hpp"
#include "write_log.hpp"

#include "formula/definition/all_fractal_list.hpp"

/**
 * Construct cSettings with the specified output format.
 * Initializes all member variables to their default values.
 */
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

/**
 * Serialize internal settings (parameters, fractal config, animations) into a text representation.
 * The output follows an INI-like format with sections [main_parameters], [fractal_N], [frames],
 * etc. A MD4 hash of the content (excluding the header) is computed for thumbnail generation.
 *
 * Returns the number of characters written to settingsText.
 */
size_t cSettings::CreateText(std::shared_ptr<const cParameterContainer> par,
	std::shared_ptr<const cFractalContainer> fractPar, std::shared_ptr<cAnimationFrames> frames,
	std::shared_ptr<cKeyframes> keyframes)
{
	WriteLog("Create settings text", 3);
	settingsText.clear();
	QString header = CreateHeader();
	settingsText += header;

	// Write description section if present and format supports it
	if ((format == formatFullText || format == formatCondensedText) && par->IfExists("description")
			&& par->Get<QString>("description") != "")
	{
		settingsText += "[description]\n";
		settingsText += par->Get<QString>("description") + "\n";
	}
	settingsText += "[main_parameters]\n";

	// Iterate over all main parameters and serialize them
	QList<QString> parameterList = par->GetListOfParameters();
	for (auto &parameterNameFromList : parameterList)
	{
		if (parameterNameFromList == "description") continue;

		// Selective saving: only include parameters in the allowlist
		if (!listOfParametersToProcess.isEmpty())
		{
			if (!listOfParametersToProcess.contains(QString("main_") + parameterNameFromList)) continue;
		}
		settingsText += CreateOneLine(par, parameterNameFromList);
	}

	// Serialize fractal parameters and animation data (not needed for app settings)
	if (format != formatAppSettings)
	{
		if (fractPar)
		{
			for (int f = 0; f < fractPar->size(); f++)
			{
				QList<QString> parameterListFractal = fractPar->at(f)->GetListOfParameters();
				QString fractalSettingsText = "";
				for (const auto &parameterNameFromFractal : parameterListFractal)
				{
					// Selective saving for fractal parameters
					if (!listOfParametersToProcess.isEmpty())
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

		// Serialize animation frames and keyframes (only for full/condensed formats)
		if (listOfParametersToProcess.isEmpty())
		{
			CreateAnimationString(settingsText, QString("frames"), frames);
			CreateAnimationString(settingsText, QString("keyframes"), keyframes);
		}
	}
	textPrepared = true;

	// Compute MD4 hash of settings content (excluding header) for thumbnail generation
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

	WriteLogString("Settings text prepared", settingsText, 3);

	return size_t(settingsText.size());
}

/**
 * Serialize animation frames or keyframes data into CSV-like text.
 * Each frame is written as a semicolon-separated row with parameter values.
 * For keyframes, an additional interpolation row is appended after all frames.
 */
void cSettings::CreateAnimationString(
	QString &text, const QString &headerText, const std::shared_ptr<cAnimationFrames> frames) const
{
	if (frames && frames->GetNumberOfFrames() > 0)
	{
		// Write section header
		text += "[" + headerText + "]\n";

		// Build CSV column headers (frame index, framesPerKeyframe, parameter columns)
		QList<cAnimationFrames::sParameterDescription> parameterList =
			frames->GetListOfUsedParameters();
		text += "frame;";
		text += "framesPerKeyframe;";

		// Write parameter column headers with type-specific suffixes (_x/_y/_z for vectors, _R/_G/_B
		// for RGB)
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

		// Serialize each frame as a CSV row
		for (int f = 0; f < frames->GetNumberOfFrames(); ++f)
		{
			text += QString::number(f) + ";";
			text += QString::number(frames->GetFrame(f).numberOfSubFrames) + ";";

			for (int i = 0; i < parameterList.size(); ++i)
			{
				if (frames->GetFrame(f).parameters.IsEmpty(
							parameterList[i].containerName + "_" + parameterList[i].parameterName))
				{
					// Write empty columns for unset parameters
					switch (parameterList[i].varType)
					{
						case parameterContainer::typeVector3:
						case parameterContainer::typeRgb: text += ";;"; break;

						case parameterContainer::typeVector4: text += ";;;"; break;
						default: text += ""; break;
					}
				}
				else
				{
					// Serialize parameter value based on its type
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
				}
				if (i != parameterList.size() - 1)
				{
					text += ";";
				}
			}
			text += "\n";
		}

		// Append interpolation types row for keyframe animations
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

/**
 * Generate the settings file header with version and format information.
 * The header is used to identify valid Mandelbulber settings files during loading.
 */
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

/**
 * Serialize a single parameter into a settings line: "name value;[script]".
 * Respects format filtering (full, condensed, net render, app settings) and
 * selective parameter lists. Skips parameters that match none of the active filters.
 */
QString cSettings::CreateOneLine(std::shared_ptr<const cParameterContainer> par, QString name) const
{
	QString text;

	cOneParameter parameter = par->GetAsOneParameter(name);
	enumParameterType parType = parameter.GetParameterType();

	// Determine if this parameter should be included based on format and parameter type
	bool selNormal =
		(format == formatFullText || format == formatCondensedText) && parType == paramStandard;

	bool selNetRender =
		(format == formatNetRender) && (parType == paramStandard || parType == paramOnlyForNet);

	// Allow selected app settings to be included in standard settings files
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

		// Only serialize non-default values (except in full/net formats or if a script is attached)
		if (!parameter.isDefaultValue() || format == formatFullText || format == formatNetRender
				|| parameter.HasScript())
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

			// formula_code is compressed and base64-encoded to save space
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

/**
 * Compress text using zlib and encode to base64.
 * Used for formula_code to reduce settings file size.
 */
QString cSettings::CompressAndCode(const QString &text) const
{
	QByteArray blob = text.toUtf8();
	QByteArray compressedBlob = qCompress(blob, 9);
	return compressedBlob.toBase64();
}

/**
 * Decode base64 data and decompress using zlib.
 * Reverse operation of CompressAndCode.
 */
QString cSettings::DecodeAndDecompress(const QString &text) const
{
	QByteArray compressedBlob = QByteArray::fromBase64(text.toUtf8());
	QByteArray blob = qUncompress(compressedBlob);
	return QString(blob);
}

/**
 * Save the generated settings text to a file.
 * Returns true on success, false if the file cannot be opened.
 */
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

/**
 * Copy the generated settings text to the system clipboard.
 */
void cSettings::SaveToClipboard() const
{
	WriteLog("Save settings to clipboard", 2);
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(settingsText);
}

/**
 * Load settings text from a file and compute its hash.
 * Returns true on success, false if the file cannot be opened.
 */
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

		// Compute MD4 hash of loaded settings for thumbnail generation
		QCryptographicHash hashCrypt(QCryptographicHash::Md4);
		hashCrypt.addData(settingsText.toLocal8Bit());
		hash = hashCrypt.result();

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

/**
 * Load settings from a QString and compute its hash.
 * Returns true on success.
 */
bool cSettings::LoadFromString(const QString &_settingsText)
{
	settingsText = _settingsText;
	textPrepared = true;

	QCryptographicHash hashCrypt(QCryptographicHash::Md4);
	hashCrypt.addData(settingsText.toLocal8Bit());
	hash = hashCrypt.result();
	return true;
}

/**
 * Load settings text from the system clipboard.
 * Returns true on success, false if clipboard is empty or unavailable.
 */
bool cSettings::LoadFromClipboard()
{
	WriteLog("Load settings from clipboard", 2);
	QClipboard *clipboard = QApplication::clipboard();
	return LoadFromString(clipboard->text());
}

/**
 * Parse the settings file header to extract version and format information.
 * Validates the file structure and updates the format member variable.
 * Throws QString on error (caught by caller).
 */
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

			// Determine format from the third header line
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

/**
 * Parse settings text and populate the parameter containers.
 * Handles section-based parsing, legacy compatibility migrations, frame/keyframe decoding,
 * and audio file loading. Returns true on success.
 */
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

	// Inject temporary legacy params for old settings files (< v2.35).
	// These allow DecodeOneLine to store values; MigrateToObjectsTree() will consume and delete them.
	if (fileVersion < 2.35)
	{
		InjectTemporaryLegacyBooleanParams(par);
		InjectTemporaryLegacyFractalParams(par);
		InjectTemporaryLegacyFormulaTransformParams(fractPar);
		InjectTemporaryLegacyFormulaMaterialIdParams(par);
	}

	int errorCount = 0;
	int csvLine = 0;

	QString section;
	if (textPrepared)
	{
		// Reset all parameters to defaults if not doing selective loading
		if (listOfParametersToProcess.isEmpty())
		{
			par->ResetAllToDefault();
			if (fractPar)
			{
				for (int i = 0; i < fractPar->size(); i++)
					fractPar->at(i)->ResetAllToDefault();
			}
			DeleteAllPrimitiveParams(par);
			listOfLoadedPrimitives.clear();
			DeleteAllMaterialParams(par);
			DeleteAllLightParams(par);
			DeleteAllNodeParams(par);
			// Only initialize nodes that are actually defined in this .fract file.
			// Scanning the text prevents creating a phantom node_0001 when the file
			// only defines node_0002+ (which would duplicate the fractal with objectid=1).
			QSet<int> definedNodeIds;
			for (const QString &line : separatedText)
			{
				QString trimmed = line.trimmed();
				if (trimmed.startsWith("node_") && trimmed.contains("_definition"))
				{
					QStringList parts = trimmed.split('_');
					if (parts.size() >= 3)
					{
						bool ok = false;
						int nid = parts[1].toInt(&ok);
						if (ok && nid > 0) definedNodeIds.insert(nid);
					}
				}
			}
			if (definedNodeIds.isEmpty())
			{
				InitNodeParams(1, par);
			}
			else
			{
				for (int nid : definedNodeIds)
					InitNodeParams(nid, par);
			}

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

		// Temporary containers used during frame/keyframe decoding
		std::shared_ptr<cParameterContainer> parTemp(new cParameterContainer());
		std::shared_ptr<cFractalContainer> fractTemp(new cFractalContainer());

		// Parse each line, dispatching to the appropriate handler based on section
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
				// Concatenate multi-line description
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
					QString decodeLine = line.trimmed();
					bool decodedAsLegacyFractalParam = false;

					// Try to resolve legacy fractal parameters (old format: param_index)
					if (fileVersion < 2.35 && fractPar && !decodeLine.isEmpty())
					{
						std::shared_ptr<cParameterContainer> targetContainer;
						QString resolvedLine;
						if (TryResolveLegacyFractalParam(
									decodeLine, par, fractPar, resolvedLine, targetContainer))
						{
							decodeLine = resolvedLine;
							result = DecodeOneLine(targetContainer, decodeLine);
							decodedAsLegacyFractalParam = true;
						}
					}

					// Apply selective loading filter for main parameters
					if (!decodedAsLegacyFractalParam && !listOfParametersToProcess.isEmpty())
					{
						int firstSpace = decodeLine.indexOf(' ');
						QString parameterName = decodeLine.left(firstSpace);

						// Override fractal index if forced
						if (forcedFractalFormulaIndex > 0)
						{
							bool conversionOK = false;
							parameterName.right(1).toInt(&conversionOK);
							if (conversionOK)
							{
								QString digit = QString::number(forcedFractalFormulaIndex);
								decodeLine[firstSpace - 1] = digit[0];
								parameterName[firstSpace - 1] = digit[0];
							}
						}

						if (!listOfParametersToProcess.contains(QString("main_") + parameterName)) continue;
					}

					if (!decodedAsLegacyFractalParam) result = DecodeOneLine(par, decodeLine);
				}
				else if (section.contains("fractal"))
				{
					// Section names: "fractal_1", "fractal_2", ... (1-based in file, 0-based internally)
					int fractalIndex = section.mid(8).toInt() - 1;
					if (forcedFractalFormulaIndex > 0) fractalIndex = forcedFractalFormulaIndex - 1;

					if (!listOfParametersToProcess.isEmpty())
					{
						int firstSpace = line.indexOf(' ');
						QString parameterName = line.left(firstSpace);
						if (forcedFractalFormulaIndex == -1
								&& !listOfParametersToProcess.contains(
									QString("fractal%1_").arg(fractalIndex) + parameterName))
							continue;
					}

					if (fractPar)
					{
						fractPar->ensureCapacity(fractalIndex);
						result = DecodeOneLine(fractPar->at(fractalIndex), line);
					}
				}
				else if (section == QString("frames"))
				{
					if (listOfParametersToProcess.isEmpty() && frames)
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
				else if (section == QString("keyframes"))
				{
					if (listOfParametersToProcess.isEmpty() && keyframes)
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

		// Add default camera animation parameters if keyframes exist but are empty
		if (keyframes && listOfParametersToProcess.isEmpty())
		{
			if (keyframes->GetListOfUsedParameters().size() == 0)
			{
				keyframes->AddAnimatedParameter("camera", par->GetAsOneParameter("camera"), par);
				keyframes->AddAnimatedParameter("target", par->GetAsOneParameter("target"), par);
				keyframes->AddAnimatedParameter("camera_top", par->GetAsOneParameter("camera_top"), par);
			}
		}

		// Ensure at least one material is defined
		if (format != formatAppSettings)
		{
			CheckIfMaterialsAreDefined(par);
		}

		// Process audio-linked parameters after animation data is loaded
		if (listOfParametersToProcess.isEmpty())
		{
			if (keyframes && linesWithSoundParameters.length() > 0)
			{
				foundAnimSoundParameters = true;
				for (const auto &linesWithSoundParameter : linesWithSoundParameters)
				{
					bool result = DecodeOneLine(par, linesWithSoundParameter);
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

		// Apply version-specific compatibility migrations
		if (format != formatAppSettings)
		{
			Compatibility2(par, fractPar);
			Compatibility3(keyframes, par, fractPar);
		}

		// Load audio files referenced by animation/keyframe data
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

/**
 * Check if any material parameters are defined. If none are found, initialize material 1.
 * Returns true if materials were already defined, false if a new one was initialized.
 */
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

	// If formula_material_id references material 1 but it is not defined, mark as not found
	if (par->IfExists("formula_material_id"))
	{
		if (par->Get<int>("formula_material_id") == 1 && !par->IfExists("mat1_is_defined"))
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

/**
 * Parse a single settings line and set the corresponding parameter value.
 * Handles parameter name/value extraction, quote stripping, compatibility transformations,
 * lazy initialization of primitives/materials/lights/nodes, and script attachment.
 * Returns true on success, false on error.
 */
bool cSettings::DecodeOneLine(std::shared_ptr<cParameterContainer> par, QString line)
{
	line = line.trimmed();
	if (line.isEmpty()) return true;

	int firstSpace = line.indexOf(' ');
	int semicolon = line.indexOf(';');
	if (firstSpace < 0 || semicolon < 0 || semicolon <= firstSpace) return false;

	QString parameterName = line.left(firstSpace).trimmed();
	QString value = line.mid(firstSpace + 1, semicolon - firstSpace - 1).trimmed();
	QString script;

	// Strip enclosing quotes from parameter name and value
	auto stripEnclosingQuotes = [](const QString &text) -> QString
	{
		if (text.size() >= 2)
		{
			const QChar first = text.at(0);
			const QChar last = text.at(text.size() - 1);
			if ((first == '"' && last == '"') || (first == '\'' && last == '\''))
			{
				return text.mid(1, text.size() - 2).trimmed();
			}
		}
		return text;
	};
	parameterName = stripEnclosingQuotes(parameterName);
	value = stripEnclosingQuotes(value);

	// Extract optional script portion (after the second semicolon)
	if (semicolon < line.length() - 2)
	{
		int semicolon2 = line.indexOf(';', semicolon + 1);
		script = line.mid(semicolon + 1, semicolon2 - semicolon - 1);
	}

	// Apply version-specific name/value transformations
	Compatibility(parameterName, value);

	// Skip parameter if compatibility handler marked it for skipping
	if (parameterName == "skip") return true;

	// Lazy-initialize primitive parameters if this is a new primitive parameter
	if (parameterName.left(parameterName.indexOf('_')) == "primitive")
	{
		QStringList split = parameterName.split('_');
		if (split.size() >= 3)
		{
			QString primitiveName = split.at(0) + "_" + split.at(1) + "_" + split.at(2);
			// Initialize only when the primitive is not yet known to the container. An unknown
			// parameter of an already initialized primitive (typo or parameter removed in newer
			// versions) must not run InitPrimitiveParams() again, otherwise every existing
			// parameter of that primitive is reported as duplicated.
			if (!par->IfExists(primitiveName + "_enabled"))
			{
				int primitiveIndex = split.at(2).toInt();
				fractal::enumObjectType objectType = cPrimitives::PrimitiveNameToEnum(split.at(1));
				sPrimitiveItem item(objectType, primitiveIndex, primitiveName, split.at(1));
				InitPrimitiveParams(item, par);
				if (!listOfLoadedPrimitives.contains(primitiveName))
					listOfLoadedPrimitives.append(primitiveName);

				// Inject temporary legacy transform params for old settings files (< v2.35).
				// MigrateToObjectsTree() will migrate them to node-based params and delete them.
				if (fileVersion < 2.35)
				{
					InjectTemporaryLegacyPrimitiveTransformParams(par, primitiveIndex);
					// Inject temporary legacy node params needed during decode for migration.
					// MigrateToObjectsTree() will migrate them to node-based params and delete them.
					InjectTemporaryLegacyPrimitiveNodeParams(par, primitiveName);
				}
			}
		}
	}

	// Lazy-initialize material parameters if this is a new material parameter
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

	// Lazy-initialize light parameters if this is a new light parameter
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

	// Lazy-initialize node parameters if this is a new node parameter
	if (parameterName.left(5) == "node_")
	{
		if (!par->IfExists(parameterName))
		{
			QStringList split = parameterName.split('_');
			if (split.size() >= 3)
			{
				bool conversionOK = false;
				int nodeId = split.at(1).toInt(&conversionOK);
				if (conversionOK && nodeId > 0)
				{
					static const QStringList validNodeParams = {"definition", "position", "rotation", "scale",
						"repeat", "material", "detail_level_multiplier", "julia_mode", "julia_c",
						"fractal_constant_factor", "initial_waxis", "smooth_de_combine_enable",
						"smooth_de_combine_distance", "formula_maxiter"};
					QString shortName = split.mid(2).join('_');
					if (validNodeParams.contains(shortName))
					{
						InitNodeParams(nodeId, par);
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
		}
	}

	// Defer audio-linked parameter processing until after animation data is loaded
	if (parameterName.left(9) == "animsound")
	{
		if (!foundAnimSoundParameters)
		{
			linesWithSoundParameters.append(line);
			return true;
		}
	}

	// Ignore flight animation audio parameters
	if (parameterName.left(15) == "flightanimsound")
	{
		return true;
	}

	// Ignore temporary legacy parameters leaked into settings files by the development builds
	// of the objects-tree migration (before v2.35). Those names were injected by
	// InjectTemporaryLegacy*Params() while reading old files and some of them were not removed
	// afterwards, so they got saved into .fract files and into the settings text sent to
	// NetRender clients. They are not valid parameters in files declaring v2.35 or newer, and
	// counting them as decoding errors would abort the whole load (the [keyframes] section is
	// stored at the end of the text, so it would be silently dropped).
	if (IsTemporaryLegacyParamName(parameterName) && !par->IfExists(parameterName)) return true;

	// Validate parameter type and set value
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

		// Decompress formula_code if needed
		if (parameterName == "formula_code")
		{
			value = DecodeAndDecompress(value);
		}

		// Convert enumeration string to index
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

/**
 * Detect if a line is a section header (e.g. "[main_parameters]").
 * Updates section with the section name and returns true if detected.
 */
bool cSettings::CheckSection(QString text, QString &section)
{
	if (text.left(1) == "[" && text.right(1) == "]")
	{
		section = text.mid(1, text.length() - 2);
		return true;
	}
	return false;
}

/**
 * Parse the animation frames/keyframes header row (column names).
 * Detects parameter types (vector3, vector4, RGB, scalar) from column suffixes (_x/_y/_z,
 * _R/_G/_B). Registers each parameter with the frames object for later decoding.
 */
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

					// Convert old parameter names to new format (fileVersion < 2.35)
					// Mirrors MigrateLegacyParamsToFractal() in Compatibility2()
					if (fileVersion < 2.35)
					{
						QString converted = ConvertLegacyAnimationParamName(fullParameterName);
						if (converted != fullParameterName)
						{
							lineSplit[i] = converted;
							fullParameterName = converted;
						}
					}

					// Detect vector4 type (_x suffix with _y, _z, _w following)
					if (lastTwo == "_x")
					{
						int firstUnderscore = fullParameterName.indexOf('_');
						QString containerName = fullParameterName.left(firstUnderscore);
						QString parameterName = fullParameterName.mid(firstUnderscore + 1);
						std::shared_ptr<cParameterContainer> selectedContainer =
							cAnimationFrames::ContainerSelector(containerName, par, fractPar);

						if (!selectedContainer->IfExists(parameterName))
						{
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

							// Detect vector3 type (_x suffix with _y, _z following)
							if (!isCVector4 && i + 2 < lineSplit.size())
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
					// Detect RGB type (_R suffix with _G, _B following)
					else if (lastTwo == "_R")
					{
						int firstUnderscore = fullParameterName.indexOf('_');
						QString containerName = fullParameterName.left(firstUnderscore);
						QString parameterName = fullParameterName.mid(firstUnderscore + 1);
						std::shared_ptr<cParameterContainer> selectedContainer =
							cAnimationFrames::ContainerSelector(containerName, par, fractPar);

						if (!selectedContainer->IfExists(parameterName))
						{
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

				// Apply compatibility transformations to parameter names
				int firstUnderscore = fullParameterName.indexOf('_');
				QString containerName = fullParameterName.left(firstUnderscore);
				QString parameterName = fullParameterName.mid(firstUnderscore + 1);
				QString value = "";
				Compatibility(parameterName, value);
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

/**
 * Parse a single animation frames/keyframes data row.
 * Handles interpolation type row for keyframes, or frame data with vector/RGB/scalar values.
 * Returns true on success, false on error.
 */
bool cSettings::DecodeFramesLine(QString line, std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> fractPar, std::shared_ptr<cAnimationFrames> frames)
{
	QStringList lineSplit = line.split(';');
	QList<cAnimationFrames::sParameterDescription> parameterList = frames->GetListOfUsedParameters();
	int column = 0;
	int numberOfSubFrames = 0;

	try
	{
		// Handle interpolation types row (appended after all keyframe data)
		if (lineSplit.size() > 0 && lineSplit[0] == QString("interpolation"))
		{
			if (lineSplit.size() - 1 == parameterList.size())
			{
				// the interpolation row is created only for keyframes, so frames must be a cKeyframes
				std::shared_ptr<cKeyframes> keyframes = std::dynamic_pointer_cast<cKeyframes>(frames);
				if (!keyframes)
				{
					throw QObject::tr("Interpolation row is allowed only in the keyframes section");
				}

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
					keyframes->ChangeMorphType(i, morphType);
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

					if (lineSplit[column].isEmpty())
					{
						container->SetEmpty(parameterName);
						switch (type)
						{
							case typeVector3: column += 2; break;
							case typeVector4: column += 3; break;
							case typeRgb: column += 2; break;
							default: break;
						}
					}
					else
					{
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
	// Returns the prepared settings text, or empty string if not yet prepared.
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
	// Converts decimal separators in a numeric string to match the current system locale.
	QString txtOut;
	// Convert '.' to ',' for locales that use comma as decimal separator
	if (systemData.decimalPoint == ',') txtOut = txt.replace('.', ',');
	// Convert ',' to '.' for locales that use dot as decimal separator
	if (systemData.decimalPoint == '.') txtOut = txt.replace(',', '.');
	return txtOut;
}
