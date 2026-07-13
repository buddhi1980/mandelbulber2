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
 * Compatibility handler for settings files older than v2.25.
 * Converts old light rotation parameters (alpha/beta) into the new unified rotation vector.
 */
void cSettings::Compatibility3(const std::shared_ptr<cKeyframes> &keyframes,
	const std::shared_ptr<cParameterContainer> &par,
	const std::shared_ptr<cFractalContainer> &fractPar)
{
	if (fileVersion < 2.25 && keyframes)
	{
		// Convert old alpha/beta light rotation parameters to the new rotation vector
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
		InjectTemporaryLegacyJuliaParams(par);
		InjectTemporaryLegacyFormulaTransformParams(fractPar);
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
			InitNodeParams(1, par);

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
		if (!par->IfExists(parameterName))
		{
			QStringList split = parameterName.split('_');
			QString primitiveName = split.at(0) + "_" + split.at(1) + "_" + split.at(2);
			int primitiveIndex = split.at(2).toInt();
			fractal::enumObjectType objectType = cPrimitives::PrimitiveNameToEnum(split.at(1));
			sPrimitiveItem item(objectType, primitiveIndex, primitiveName, split.at(1));
			InitPrimitiveParams(item, par);
			listOfLoadedPrimitives.append(primitiveName);

			// Inject temporary legacy transform params for old settings files (< v2.35).
			// MigrateToObjectsTree() will migrate them to node-based params and delete them.
			if (fileVersion < 2.35)
			{
				InjectTemporaryLegacyPrimitiveTransformParams(par, primitiveIndex);
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
 * Apply version-specific compatibility transformations to parameter names and values.
 * Handles renames, value conversions, and deprecated parameter handling across all
 * Mandelbulber versions. Parameters marked "skip" are ignored by the caller.
 */
void cSettings::Compatibility(QString &name, QString &value) const
{
	// === v2.01 compatibility ===
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

	// === v2.04 compatibility ===
	if (fileVersion <= 2.04)
	{
		if (name == QString("fractal_constant_factor"))
		{
			QString newValue = value + " " + value + " " + value;
			value = newValue;
		}
	}

	// === v2.06 compatibility ===
	if (fileVersion <= 2.06)
	{
		if (name == QString("linear_DE_mode"))
		{
			name = QString("delta_DE_function");
			value = QString("1");
		}
	}

	// === v2.071 compatibility: rename material parameters to mat1_* prefix ===
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

	// === v2.09 compatibility: swap delta_DE_function values ===
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

	// === v2.12 compatibility: rename gpu_ to opencl_ ===
	if (fileVersion < 2.12)
	{
		name.replace("gpu_", "opencl_");
	}

	// === v2.13 compatibility: primitive_water amplitude rename ===
	if (fileVersion < 2.13)
	{
		if (name.contains("primitive_water"))
		{
			name.replace("amplitude", "relative_amplitude");
		}
	}

	// === v2.19 compatibility: surface_color_palette -> surface_color_gradient ===
	if (fileVersion < 2.19)
	{
		if (name.contains("surface_color_palette"))
		{
			name.replace("surface_color_palette", "surface_color_gradient");

			QStringList split = value.split(" ");
			int numberOfColors = split.size();

			// Skip trailing invalid color entries
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

	// === v2.25 compatibility: rename main_light_* to light1_*, aux_light_* to light2_*+ ===
	if (fileVersion < 2.25)
	{
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

		// Handle aux_light_* -> light2_*+ renaming
		if (name.contains("aux_light"))
		{
			if (name.contains("visibility_size"))
				name.replace("aux_light_visibility_size", "light2_size");
			else if (name.contains("visibility"))
				name.replace("aux_light_visibility", "light2_visibility");
			else if (name.contains("aux_light_place_behind"))
			{
				// No mapping for this parameter
			}
			else
			{
				QStringList split = name.split("_");
				int lightIndex = split.last().toInt();
				QString prefix = QString("light%1").arg(lightIndex + 1);

				if (split.at(2) == "intensity")
					name = QString("%1_intensity").arg(prefix);
				else if (split.at(2) == "position")
					name = QString("%1_position").arg(prefix);
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

	// === v2.28 compatibility: rename random_lights_one_color_enable ===
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

/**
 * Apply version-specific compatibility migrations for settings files.
 * Handles DE function normalization, primitive-to-material migration, gradient renames,
 * FOV conversion, light parameter migrations, and objects tree migration (v2.35+).
 */
void cSettings::Compatibility2(
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fract)
{
	// === v2.06: normalize DE function, enable IFS rotation ===
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

	// === v2.071: migrate loaded primitives to materials ===
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

	// === v2.12: iteration fog boost, water relative amplitude fix ===
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

	// === v2.19: palette offset/speed normalization, gradient migration ===
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

	// === v2.20: delta_DE_method -> delta_DE_function mapping ===
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

	// === v2.21: FOV conversion (radians -> degrees) ===
	if (fileVersion < 2.21)
	{
		params::enumPerspectiveType perspectiveType =
			params::enumPerspectiveType(par->Get<int>("perspective_type"));
		double fov = par->Get<double>("fov");
		if (fov == 53.13) fov = 1.0;

		double fovDegrees = 0.0;
		switch (perspectiveType)
		{
			case params::perspThreePoint: fovDegrees = atan(fov / 2.0) * 360.0 / M_PI; break;
			case params::perspFishEye:
			case params::perspFishEyeCut: fovDegrees = fov * 180.0; break;
			case params::perspEquirectangular: fovDegrees = fov * 360.0; break;
		}
		par->Set("fov", fovDegrees);
	}

	// === v2.22: enable IFS edge if non-zero ===
	if (fileVersion < 2.22)
	{
		if (fract)
		{
			for (int i = 0; i < fract->size(); i++)
			{
				if (fract->at(i)->Get<CVector3>("IFS_edge").Length() > 0)
				{
					fract->at(i)->Set("IFS_edge_enabled", true);
				}
			}
		}
	}

	// === v2.25: light1 rotation vector, light intensity/size corrections, random lights ===
	if (fileVersion < 2.25)
	{
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

		// Copy light visibility and intensity from light2 to other aux lights
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

		// Correct intensity and size for aux lights
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

		// Correct random lights intensity and size
		if (par->Get<bool>("random_lights_group"))
		{
			int numberOfRandomLights = par->Get<int>("random_lights_number");
			par->Set("random_lights_intensity",
				par->Get<double>("random_lights_intensity") / (numberOfRandomLights / 4 + 4));
			par->Set(
				"random_lights_size", par->Get<double>("light2_size") * (sqrt(numberOfRandomLights) / 4));
		}
	}

	// === v2.28: adjust light1 intensity for iteration fog ===
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

	// === v2.29: migrate boolean_operators to per-fractal formula_maxiter ===
	if (fileVersion < 2.29)
	{
		if (par->IfExists("boolean_operators") && par->Get<bool>("boolean_operators"))
		{
			int maxiter = par->Get<int>("N");
			for (int i = 1; i <= fract->size(); i++)
			{
				par->Set("formula_maxiter", i, maxiter);
			}
		}
	}

	// === v2.35: migrate to objects tree (node-based hierarchy) ===
	if (fileVersion < 2.35)
	{
		if (fract)
		{
			DeleteAllNodeParams(par);
			int nextGroupObjectId = 100;
			MigrateToObjectsTree(par, fract, nextGroupObjectId);
		}
		DeleteTemporaryLegacyPrimitiveTransformParams(par);
	}

	// === compatibility: migrate "formula" (without index) to "formula_N" in fractal sections ===
	if (fract)
	{
		for (int i = 0; i < fract->size(); i++)
		{
			QString formulaParamName = QString("formula_%1").arg(i + 1);
			if (!fract->at(i)->IfExists(formulaParamName))
			{
				fract->at(i)->addParam(
					formulaParamName, int(fractal::mandelbulb), morphNone, paramStandard);
			}
			if (fract->at(i)->IfExists("formula") && !fract->at(i)->isDefaultValue("formula"))
			{
				fract->at(i)->Set(formulaParamName, fract->at(i)->Get<int>("formula"));
			}
		}
	}
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

void cSettings::PreCompatibilityMaterials(int matIndex, std::shared_ptr<cParameterContainer> par)
{
	// Sets default metallic/material params for file versions prior to 2.15.
	if (fileVersion < 2.15)
	{
		// Set default metallic value to false for backward compatibility
		par->Set(cMaterial::Name("metallic", matIndex), false);
		// Set default specular value to 1.0 for backward compatibility
		par->Set(cMaterial::Name("specular", matIndex), 1.0);
		// Set default specular_width value to 1.0 for backward compatibility
		par->Set(cMaterial::Name("specular_width", matIndex), 1.0);
	}
}

QStringList cSettings::GetLegacyPrimitiveTypes()
{
	// Returns the list of legacy primitive type names used in old file formats.
	return {"sphere", "box", "cylinder", "cone", "plane", "torus", "rectangle", "circle", "water",
		"prism", "ellipsoid"};
}

void cSettings::InjectTemporaryLegacyBooleanParams(std::shared_ptr<cParameterContainer> par)
{
	// Injects temporary boolean params from the legacy flat format into the parameter
	// container. Ensure top-level boolean_operators flag exists
	if (!par->IfExists("boolean_operators"))
		par->addParam("boolean_operators", false, morphNone, paramStandard);
	// Inject per-fractal boolean_operator and dont_add_c_constant params
	const int maxLegacyFractals = 9;
	for (int i = 1; i < maxLegacyFractals; i++)
	{
		QString name = QString("boolean_operator_%1").arg(i);
		if (!par->IfExists(name)) par->addParam(name, 1, morphLinear, paramStandard);

		name = QString("dont_add_c_constant_%1").arg(i);
		if (!par->IfExists(name)) par->addParam(name, false, morphLinear, paramStandard);
	}
}

void cSettings::InjectTemporaryLegacyJuliaParams(std::shared_ptr<cParameterContainer> par)
{
	// Injects temporary per-fractal julia_mode, julia_c, and fractal_constant_factor params
	// from the legacy flat format into the parameter container.
	const int maxLegacyFractals = 9;
	for (int i = 1; i <= maxLegacyFractals; i++)
	{
		QString name = QString("fractal_constant_factor_%1").arg(i);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(1.0, 1.0, 1.0), morphLinear, paramStandard);
	}
	for (int i = 1; i <= maxLegacyFractals; i++)
	{
		QString paramName = QString("julia_mode_%1").arg(i);
		if (!par->IfExists(paramName)) par->addParam(paramName, false, morphLinear, paramStandard);
		paramName = QString("julia_c_%1").arg(i);
		if (!par->IfExists(paramName))
			par->addParam(paramName, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	}
}

void cSettings::InjectTemporaryLegacyFormulaTransformParams(
	std::shared_ptr<cFractalContainer> fractPar)
{
	// Injects temporary legacy formula transform params into each fractal container.
	// These are needed by TryResolveLegacyFractalParam to resolve formula_position_N,
	// formula_rotation_N, formula_repeat_N, formula_scale_N params from old .fract files.
	if (fractPar)
	{
		const int maxLegacyFractals = 9;
		for (int i = 0; i < maxLegacyFractals; i++)
		{
			fractPar->ensureCapacity(i);
			if (!fractPar->at(i)->IfExists("formula_material_id"))
				fractPar->at(i)->addParam("formula_material_id", 1, morphLinear, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_position"))
				fractPar->at(i)->addParam(
					"formula_position", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_rotation"))
				fractPar->at(i)->addParam(
					"formula_rotation", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_repeat"))
				fractPar->at(i)->addParam(
					"formula_repeat", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_scale"))
				fractPar->at(i)->addParam("formula_scale", 1.0, morphAkima, paramStandard);
		}
	}
}

void cSettings::InjectTemporaryLegacyPrimitiveTransformParams(
	std::shared_ptr<cParameterContainer> par, int primitiveIndex)
{
	// Injects temporary position/rotation/scale params for a legacy primitive into the parameter
	// container.
	QStringList legacyTypes = GetLegacyPrimitiveTypes();
	for (const QString &type : legacyTypes)
	{
		// Inject position param with default zero vector
		QString name = QString("primitive_%1_%2_position").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		// Inject rotation param with default zero vector
		name = QString("primitive_%1_%2_rotation").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
		// Inject scale param with default (1,1,1) vector
		name = QString("primitive_%1_%2_scale").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
		// Inject repeat param with default zero vector (legacy transform param)
		name = QString("primitive_%1_%2_repeat").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	}
}

bool cSettings::TryResolveLegacyFractalParam(const QString &decodeLine,
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fractPar,
	QString &resolvedLine, std::shared_ptr<cParameterContainer> &targetContainer)
{
	// Resolves a legacy fractal param name (e.g. "fractal_xxx_1") to the base param name and target
	// fractal container. Early exit if fractPar is null or decodeLine is empty
	if (!fractPar || decodeLine.isEmpty()) return false;

	// Extract the parameter name (before the first space)
	int firstSpace = decodeLine.indexOf(' ');
	if (firstSpace <= 0) return false;

	QString rawName = decodeLine.left(firstSpace).trimmed();
	if (rawName.size() < 2) return false;

	// Strip surrounding quotes (single or double) from the parameter name
	if (((rawName.at(0) == '"' && rawName.at(rawName.size() - 1) == '"')
				|| (rawName.at(0) == '\'' && rawName.at(rawName.size() - 1) == '\'')))
	{
		rawName = rawName.mid(1, rawName.size() - 2).trimmed();
	}

	// Extract the fractal index from the trailing _N suffix
	int lastUnderscore = rawName.lastIndexOf('_');
	if (lastUnderscore <= 0) return false;

	bool conversionOK = false;
	int fractalIndex = rawName.mid(lastUnderscore + 1).toInt(&conversionOK) - 1;
	QString baseParam = rawName.left(lastUnderscore);

	// Ensure the target fractal container exists before checking the base parameter
	if (conversionOK && fractalIndex >= 0)
	{
		fractPar->ensureCapacity(fractalIndex);
	}

	// Check if the legacy param does not exist in the top-level container
	// and the base param exists in the target fractal container
	if (conversionOK && fractalIndex >= 0 && fractalIndex < fractPar->size()
			&& !par->IfExists(rawName) && fractPar->at(fractalIndex)->IfExists(baseParam))
	{
		// Build the resolved line with the base param name and update the target container
		resolvedLine = baseParam + decodeLine.mid(firstSpace);
		targetContainer = fractPar->at(fractalIndex);
		return true;
	}

	return false;
}

void cSettings::MigrateLegacyParamsToFractal(
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fract)
{
	// Migrates legacy flat params (dont_add_c_constant, fractal_constant_factor, julia_mode, julia_c)
	// to the new fractal-based structure. Migrate per-fractal dont_add_c_constant params
	for (int i = 1; i <= fract->size(); i++)
	{
		QString oldParamName = QString("dont_add_c_constant_%1").arg(i);
		if (par->IfExists(oldParamName))
		{
			if (par->Get<bool>(oldParamName))
			{
				fract->at(i - 1)->Set("dont_add_c_constant", true);
			}
			// Remove the legacy param after migration
			par->DeleteParameter(oldParamName);
		}
	}

	// Migrate per-fractal fractal_constant_factor params
	for (int i = 1; i <= fract->size(); i++)
	{
		QString oldParamName = QString("fractal_constant_factor_%1").arg(i);
		if (par->IfExists(oldParamName))
		{
			if (!par->isDefaultValue(oldParamName))
			{
				fract->at(i - 1)->Set("fractal_constant_factor", par->Get<CVector3>(oldParamName));
			}
			// Remove the legacy param after migration
			par->DeleteParameter(oldParamName);
		}
	}

	// Migrate per-fractal julia_mode and julia_c params
	for (int i = 1; i <= fract->size(); i++)
	{
		QString juliaModeParamName = QString("julia_mode_%1").arg(i);
		if (par->IfExists(juliaModeParamName))
		{
			if (par->Get<bool>(juliaModeParamName))
			{
				fract->at(i - 1)->Set("julia_mode", true);
			}
			// Remove the legacy param after migration
			par->DeleteParameter(juliaModeParamName);
		}

		QString juliaCParamName = QString("julia_c_%1").arg(i);
		if (par->IfExists(juliaCParamName))
		{
			if (!par->isDefaultValue(juliaCParamName))
			{
				fract->at(i - 1)->Set("julia_c", par->Get<CVector3>(juliaCParamName));
			}
			// Remove the legacy param after migration
			par->DeleteParameter(juliaCParamName);
		}
	}

	bool booleanMode = par->IfExists("boolean_operators") && par->Get<bool>("boolean_operators");

	// Migrate global N to formula_maxiter for non-boolean mode files
	if (!booleanMode && par->IfExists("N"))
	{
		int maxiter = par->Get<int>("N");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("formula_maxiter", maxiter);
		}
	}

	// Handle global fractal_constant_factor when not in boolean mode
	if (!booleanMode && par->IfExists("fractal_constant_factor")
			&& !par->isDefaultValue("fractal_constant_factor"))
	{
		CVector3 constFactor = par->Get<CVector3>("fractal_constant_factor");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("fractal_constant_factor", constFactor);
		}
	}

	// Handle global julia_mode and julia_c
	if (par->IfExists("julia_mode") && !par->isDefaultValue("julia_mode"))
	{
		bool juliaMode = par->Get<bool>("julia_mode");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("julia_mode", juliaMode);
		}
	}
	if (par->IfExists("julia_c") && !par->isDefaultValue("julia_c"))
	{
		CVector3 juliaC = par->Get<CVector3>("julia_c");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("julia_c", juliaC);
		}
	}
}

static QString GetFormulaName(int formulaEnum)
{
	for (cAbstractFractal *f : newFractalList)
	{
		if (int(f->getInternalId()) == formulaEnum)
		{
			return f->getInternalName();
		}
	}
	return "fractal";
}

static enumNodeType ToNodeType(int boolOp)
{
	switch (boolOp)
	{
		case 0: return enumNodeType::booleanMul;
		case 2: return enumNodeType::booleanSub;
		default: return enumNodeType::booleanAdd;
	}
}

static enumNodeType PrimitiveOpToNodeType(int boolOp)
{
	switch (boolOp)
	{
		case int(primBooleanOperatorAND): return enumNodeType::booleanMul;
		case int(primBooleanOperatorSUB): return enumNodeType::booleanSub;
		case int(primBooleanOperatorRevSUB): return enumNodeType::booleanSub;
		default: return enumNodeType::booleanAdd;
	}
}

static QString MakeNodeDefinition(const QString &formulaName, int nodeId, enumNodeType type,
	int parentId, int objectId, int displayOrder = 0)
{
	return QString("%1 %2,%2,%3,%4,%5,%6")
		.arg(formulaName)
		.arg(nodeId)
		.arg(int(type))
		.arg(parentId)
		.arg(objectId)
		.arg(displayOrder);
}

static QString NodePrefix(int nodeId)
{
	return QString("node_%1_").arg(nodeId, 4, 10, QChar('0'));
}

static QString NodeDefinitionParam(int nodeId)
{
	return NodePrefix(nodeId) + "definition";
}

static void SetNodeParent(std::shared_ptr<cParameterContainer> par, int nodeId, int parentId)
{
	const QString defParam = NodeDefinitionParam(nodeId);
	if (!par->IfExists(defParam)) return;
	QStringList parts = par->Get<QString>(defParam).split(',');
	if (parts.size() < 5) return;
	parts[3] = QString::number(parentId);
	par->Set(defParam, parts.join(","));
}

static void CopyFormulaTransform(std::shared_ptr<cParameterContainer> par, const QString &prefix,
	std::shared_ptr<cParameterContainer> fracPar)
{
	if (fracPar->IfExists("formula_position"))
		par->Set(prefix + "position", fracPar->Get<CVector3>("formula_position"));
	if (fracPar->IfExists("formula_rotation"))
		par->Set(prefix + "rotation", fracPar->Get<CVector3>("formula_rotation"));
	if (fracPar->IfExists("formula_scale"))
		par->Set(prefix + "scale", fracPar->Get<double>("formula_scale"));
	if (fracPar->IfExists("formula_repeat"))
		par->Set(prefix + "repeat", fracPar->Get<CVector3>("formula_repeat"));
	if (fracPar->IfExists("formula_material_id"))
		par->Set(prefix + "material", fracPar->Get<int>("formula_material_id"));
}

// Copy common fractal params (julia_mode, julia_c, etc.) from per-fractal container to node params.
// Used during migration to populate node-level params from legacy per-fractal params.
static void CopyCommonFractalParams(std::shared_ptr<cParameterContainer> par, const QString &prefix,
	std::shared_ptr<cParameterContainer> fracPar)
{
	if (fracPar->IfExists("julia_mode") && fracPar->Get<bool>("julia_mode"))
		par->Set(prefix + "julia_mode", true);
	if (fracPar->IfExists("julia_c")) par->Set(prefix + "julia_c", fracPar->Get<CVector3>("julia_c"));
	if (fracPar->IfExists("fractal_constant_factor"))
		par->Set(prefix + "fractal_constant_factor", fracPar->Get<CVector3>("fractal_constant_factor"));
	if (fracPar->IfExists("initial_waxis"))
		par->Set(prefix + "initial_waxis", fracPar->Get<double>("initial_waxis"));
	if (fracPar->IfExists("smooth_de_combine_enable")
			&& fracPar->Get<bool>("smooth_de_combine_enable"))
		par->Set(prefix + "smooth_de_combine_enable", true);
	if (fracPar->IfExists("smooth_de_combine_distance"))
		par->Set(
			prefix + "smooth_de_combine_distance", fracPar->Get<double>("smooth_de_combine_distance"));
	if (fracPar->IfExists("formula_maxiter"))
		par->Set(prefix + "formula_maxiter", fracPar->Get<int>("formula_maxiter"));
}

static void CopyPrimitiveTransform(
	std::shared_ptr<cParameterContainer> par, const QString &primitiveFullName, const QString &prefix)
{
	if (par->IfExists(primitiveFullName + "_position"))
		par->Set(prefix + "position", par->Get<CVector3>(primitiveFullName + "_position"));
	if (par->IfExists(primitiveFullName + "_rotation"))
		par->Set(prefix + "rotation", par->Get<CVector3>(primitiveFullName + "_rotation"));
	if (par->IfExists(primitiveFullName + "_scale"))
		par->Set(prefix + "scale", par->Get<CVector3>(primitiveFullName + "_scale"));
	if (par->IfExists(primitiveFullName + "_repeat"))
		par->Set(prefix + "repeat", par->Get<CVector3>(primitiveFullName + "_repeat"));
}

static QList<int> GetEnabledFractals(std::shared_ptr<cFractalContainer> fract)
{
	QList<int> enabledFractals;
	for (int i = 0; i < fract->size(); i++)
	{
		if (fract->at(i)->IfExists("formula")
				&& fract->at(i)->Get<int>("formula") != int(fractal::none))
		{
			enabledFractals.append(i + 1);
		}
	}
	return enabledFractals;
}

// Default object_id value assigned to primitives that were not explicitly renumbered.
static const int DefaultPrimitiveObjectId = 1234;

void cSettings::MigrateToObjectsTree(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> fract, int &nextGroupObjectId)
{
	// Scan raw settings text to find which primitives had object_id in the original file.
	// This is needed because InitPrimitiveParams adds object_id=1234 during decode,
	// so IfExists() would always return true.
	QSet<QString> primitivesWithObjectIdInFile;
	QRegularExpression re("primitive_(\\w+_(\\d+))_object_id");
	QRegularExpressionMatchIterator regexIt(re.globalMatch(settingsText));
	while (regexIt.hasNext())
	{
		QRegularExpressionMatch match = regexIt.next();
		primitivesWithObjectIdInFile.insert(match.captured(1));
	}

	// Migrates legacy flat params to the new node-based objects tree structure.
	// Phase 1: migrate transforms only for primitives with non-default object_id
	// (where a corresponding node might already exist from external files).
	QList<sPrimitiveItem> primitives = cPrimitives::GetListOfPrimitives(par);
	for (auto &primitive : primitives)
	{
		primitive.objectIdInFile = primitivesWithObjectIdInFile.contains(primitive.fullName);
	}
	for (const auto &primitive : primitives)
	{
		if (!par->IfExists(primitive.Name("enabled")) || !par->Get<bool>(primitive.Name("enabled")))
		{
			continue;
		}

		int objectId = par->Get<int>(primitive.Name("object_id"));
		// Only migrate transforms for primitives that have explicit object_id in the file
		// and that were renumbered (not using the default value).
		if (!primitive.objectIdInFile || objectId == DefaultPrimitiveObjectId)
		{
			continue;
		}

		const QString prefix = NodePrefix(objectId);

		if (par->IfExists(primitive.Name("position"))
				&& !par->isDefaultValue(primitive.Name("position")))
		{
			par->Set(prefix + "position", par->Get<CVector3>(primitive.Name("position")));
		}
		if (par->IfExists(primitive.Name("rotation"))
				&& !par->isDefaultValue(primitive.Name("rotation")))
		{
			par->Set(prefix + "rotation", par->Get<CVector3>(primitive.Name("rotation")));
		}
		if (par->IfExists(primitive.Name("scale")) && !par->isDefaultValue(primitive.Name("scale")))
		{
			par->Set(prefix + "scale", par->Get<CVector3>(primitive.Name("scale")));
		}
	}

	bool hybridMode = par->Get<bool>("hybrid_fractal_enable");
	bool booleanMode = par->IfExists("boolean_operators") && par->Get<bool>("boolean_operators");

	// Migrate legacy flat fractal params to the new fractal-based structure
	MigrateLegacyParamsToFractal(par, fract);

	QList<int> enabledFractals;

	// Handle boolean mode: build a node tree for multiple fractals
	if (booleanMode)
	{
		enabledFractals = GetEnabledFractals(fract);
		const int m = enabledFractals.size();

		// Single fractal: create a single fractal node
		if (m == 1)
		{
			InitNodeParams(1, par);
			int objectId = enabledFractals[0];
			QString formulaName = GetFormulaName(fract->at(objectId - 1)->Get<int>("formula"));
			par->Set("node_0001_definition",
				MakeNodeDefinition(formulaName, 1, enumNodeType::fractal, 0, objectId));
			CopyFormulaTransform(par, "node_0001_", fract->at(objectId - 1));
			CopyCommonFractalParams(par, "node_0001_", fract->at(objectId - 1));
		}
		// Multiple fractals: build a left-associative binary tree
		else if (m >= 2)
		{
			// Build a left-associative binary tree for m fractals and m-1 operators.
			// IDs are assigned as:
			//   Boolean nodes (innermost first): ids 1 .. m-1
			//   Fractal nodes: ids m .. 2m-1

			// Create boolean (inner) nodes
			for (int k = 0; k < m - 1; k++)
			{
				int nodeId = k + 1;
				InitNodeParams(nodeId, par);

				int slotIdx = enabledFractals[k + 1] - 1;
				if (slotIdx < 1)
				{
					slotIdx = 1;
				}
				int boolOp = par->Get<int>(QString("boolean_operator_%1").arg(slotIdx));
				enumNodeType nodeType = ToNodeType(boolOp);

				int parentId = (k == m - 2) ? 0 : (k + 2);
				par->Set(QString("node_%1_definition").arg(nodeId, 4, 10, QChar('0')),
					MakeNodeDefinition("boolean", nodeId, nodeType, parentId, nextGroupObjectId++));
				par->Set(NodePrefix(nodeId) + "material", -1);
			}

			// Create fractal (leaf) nodes and attach them to the boolean tree
			for (int i = 0; i < m; i++)
			{
				int nodeId = m + i;
				int objectId = enabledFractals[i];
				InitNodeParams(nodeId, par);

				QString formulaName = GetFormulaName(fract->at(objectId - 1)->Get<int>("formula"));
				int parentId = (i <= 1) ? 1 : i;
				par->Set(QString("node_%1_definition").arg(nodeId, 4, 10, QChar('0')),
					MakeNodeDefinition(formulaName, nodeId, enumNodeType::fractal, parentId, objectId));
				CopyFormulaTransform(par, NodePrefix(nodeId), fract->at(objectId - 1));
				CopyCommonFractalParams(par, NodePrefix(nodeId), fract->at(objectId - 1));
			}
		}
	}
	// Handle non-hybrid single-fractal mode
	else if (!hybridMode)
	{
		const int nodeId = 1;
		const int objectId = 1;
		InitNodeParams(nodeId, par);

		QString formulaName = GetFormulaName(fract->at(0)->Get<int>("formula"));
		par->Set("node_0001_definition",
			MakeNodeDefinition(formulaName, nodeId, enumNodeType::fractal, 0, objectId));
		CopyFormulaTransform(par, "node_0001_", fract->at(0));
		CopyCommonFractalParams(par, "node_0001_", fract->at(0));
	}
	// Handle hybrid mode
	else
	{
		enabledFractals = GetEnabledFractals(fract);

		if (!enabledFractals.isEmpty())
		{
			// Create a hybrid root node
			InitNodeParams(1, par);
			par->Set("node_0001_definition",
				MakeNodeDefinition("hybrid", 1, enumNodeType::hybrid, 0, nextGroupObjectId++));

			// Create fractal child nodes under the hybrid root
			for (int i = 0; i < enabledFractals.size(); i++)
			{
				int nodeId = i + 2;
				int objectId = enabledFractals[i];
				InitNodeParams(nodeId, par);

				QString formulaName = GetFormulaName(fract->at(objectId - 1)->Get<int>("formula"));
				QString prefix = NodePrefix(nodeId);
				par->Set(prefix + "definition",
					MakeNodeDefinition(formulaName, nodeId, enumNodeType::fractal, 1, objectId));
				CopyFormulaTransform(par, prefix, fract->at(objectId - 1));
				CopyCommonFractalParams(par, prefix, fract->at(objectId - 1));
			}
		}
	}

	// Sort primitives by their calculation order
	std::sort(primitives.begin(), primitives.end(),
		[&](const sPrimitiveItem &a, const sPrimitiveItem &b)
		{
			const QString aOrderName = a.fullName + "_calculation_order";
			const QString bOrderName = b.fullName + "_calculation_order";
			const bool aExists = par->IfExists(aOrderName);
			const bool bExists = par->IfExists(bOrderName);
			if (!aExists && !bExists) return false;
			if (!aExists) return true;
			if (!bExists) return false;
			return par->Get<int>(aOrderName) < par->Get<int>(bOrderName);
		});

	// Find the current root node (node with parent == 0) and the maximum node ID
	int maxNodeId = 0;
	int rootNodeId = -1;
	QStringList allParams = par->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (!paramName.startsWith("node_") || !paramName.endsWith("_definition"))
		{
			continue;
		}
		const int nodeId = paramName.section('_', 1, 1).toInt();
		if (nodeId > maxNodeId)
		{
			maxNodeId = nodeId;
		}
		QStringList parts = par->Get<QString>(paramName).split(',');
		if (parts.size() == 5 && parts[3].toInt() == 0)
		{
			rootNodeId = nodeId;
		}
	}

	// Attach each enabled primitive to the node tree
	// Generate unique userObjectId for primitives that don't have explicit object_id in file.
	int nextAutoObjectId = 200;
	for (const auto &primitive : primitives)
	{
		if (!par->IfExists(primitive.Name("enabled")) || !par->Get<bool>(primitive.Name("enabled")))
		{
			continue;
		}

		const int primitiveObjectId =
			primitive.objectIdInFile ? primitive.objectID : nextAutoObjectId++;
		// Update the parameter container so GetPrimitiveObjectId returns the unique ID.
		if (!primitive.objectIdInFile)
		{
			par->Set(primitive.Name("object_id"), primitiveObjectId);
		}
		const QString primitiveName = par->IfExists(primitive.Name("name"))
																		? par->Get<QString>(primitive.Name("name"))
																		: primitive.typeName;

		// If no root node exists yet, create one from the primitive
		if (rootNodeId < 0)
		{
			const int primitiveNodeId = ++maxNodeId;
			InitNodeParams(primitiveNodeId, par);
			par->Set(
				NodeDefinitionParam(primitiveNodeId), MakeNodeDefinition(primitiveName, primitiveNodeId,
																								enumNodeType::primitive, 0, primitiveObjectId));
			QString matParam = primitive.Name("material_id");
			if (par->IfExists(matParam))
			{
				int matId = par->Get<int>(matParam);
				if (matId > 0)
				{
					par->Set(NodePrefix(primitiveNodeId) + "material", matId);
				}
			}
			CopyPrimitiveTransform(par, primitive.fullName, NodePrefix(primitiveNodeId));
			rootNodeId = primitiveNodeId;
			continue;
		}

		// Create a boolean node wrapping the primitive and attach it to the current root
		const int boolNodeId = ++maxNodeId;
		const int primitiveNodeId = ++maxNodeId;
		const int primitiveBoolOp = par->IfExists(primitive.Name("boolean_operator"))
																	? par->Get<int>(primitive.Name("boolean_operator"))
																	: int(primBooleanOperatorOR);

		InitNodeParams(boolNodeId, par);
		par->Set(NodeDefinitionParam(boolNodeId),
			MakeNodeDefinition(
				"boolean", boolNodeId, PrimitiveOpToNodeType(primitiveBoolOp), 0, nextGroupObjectId++));
		par->Set(NodePrefix(boolNodeId) + "material", -1);

		SetNodeParent(par, rootNodeId, boolNodeId);

		InitNodeParams(primitiveNodeId, par);
		par->Set(NodeDefinitionParam(primitiveNodeId),
			MakeNodeDefinition(
				primitiveName, primitiveNodeId, enumNodeType::primitive, boolNodeId, primitiveObjectId));
		QString matParam = primitive.Name("material_id");
		if (par->IfExists(matParam))
		{
			int matId = par->Get<int>(matParam);
			if (matId > 0)
			{
				par->Set(NodePrefix(primitiveNodeId) + "material", matId);
			}
		}
		CopyPrimitiveTransform(par, primitive.fullName, NodePrefix(primitiveNodeId));

		rootNodeId = boolNodeId;
	}

	// Clean up all temporary legacy params after migration
	DeleteTemporaryLegacyBooleanParams(par);
	DeleteTemporaryLegacyJuliaParams(par);
	DeleteTemporaryLegacyFormulaTransformParams(fract);
}

void cSettings::DeleteTemporaryLegacyBooleanParams(std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary legacy boolean params that were injected during migration.
	// Remove the top-level boolean_operators flag
	if (par->IfExists("boolean_operators"))
	{
		par->DeleteParameter("boolean_operators");
	}
	// Remove per-fractal boolean_operator params
	const int maxLegacyFractals = 9;
	for (int i = 1; i < maxLegacyFractals; i++)
	{
		QString name = QString("boolean_operator_%1").arg(i);
		if (par->IfExists(name))
		{
			par->DeleteParameter(name);
		}
	}
}

void cSettings::DeleteTemporaryLegacyJuliaParams(std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary per-fractal julia_mode, julia_c, and fractal_constant_factor params.
	const int maxLegacyFractals = 9;
	for (int i = 1; i <= maxLegacyFractals; i++)
	{
		QString name = QString("fractal_constant_factor_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
		name = QString("julia_mode_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
		name = QString("julia_c_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
	}
}

void cSettings::DeleteTemporaryLegacyFormulaTransformParams(
	std::shared_ptr<cFractalContainer> fractPar)
{
	// Deletes the temporary legacy formula transform params from each fractal container.
	if (fractPar)
	{
		const int maxLegacyFractals = 9;
		for (int i = 0; i < maxLegacyFractals; i++)
		{
			if (fractPar->at(i)->IfExists("formula_material_id"))
				fractPar->at(i)->DeleteParameter("formula_material_id");
			if (fractPar->at(i)->IfExists("formula_position"))
				fractPar->at(i)->DeleteParameter("formula_position");
			if (fractPar->at(i)->IfExists("formula_rotation"))
				fractPar->at(i)->DeleteParameter("formula_rotation");
			if (fractPar->at(i)->IfExists("formula_repeat"))
				fractPar->at(i)->DeleteParameter("formula_repeat");
			if (fractPar->at(i)->IfExists("formula_scale"))
				fractPar->at(i)->DeleteParameter("formula_scale");
		}
	}
}

void cSettings::DeleteTemporaryLegacyPrimitiveTransformParams(
	std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary legacy primitive transform params (position/rotation/scale) that were
	// injected during migration.
	QStringList legacyTypes = GetLegacyPrimitiveTypes();
	const int maxLegacyFractals = 9;
	for (const QString &type : legacyTypes)
	{
		for (int i = 1; i <= maxLegacyFractals; i++)
		{
			// Delete position param
			QString name = QString("primitive_%1_%2_position").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete rotation param
			name = QString("primitive_%1_%2_rotation").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete scale param
			name = QString("primitive_%1_%2_scale").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete repeat param (legacy transform param)
			name = QString("primitive_%1_%2_repeat").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
		}
	}
}
