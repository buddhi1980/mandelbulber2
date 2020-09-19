/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * system data - place for system functions and definition of default file paths
 */

#include "system.hpp"

#include <qstylefactory.h>

#include <clocale>

#include <QTextStream>

#include "global_data.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include "render_window.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

// custom includes
#ifndef _WIN32
#include <csignal>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#if defined(__APPLE__) || defined(__MACOSX)
#include "CoreFoundation/CoreFoundation.h"
#endif

//#define CLSUPPORT

sSystem systemData;
sActualFileNames actualFileNames;

bool InitSystem()
{
	setlocale(LC_ALL, "");
	systemData.locale = QLocale::system();
	systemData.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	QLocale::setDefault(systemData.locale);

	QTextStream out(stdout);
	QTextStream outErr(stderr);

	systemData.globalTimer.start();

	systemDirectories.homeDir = QDir::toNativeSeparators(QDir::homePath() + QDir::separator());
#ifdef _WIN32 /* WINDOWS */
	systemDirectories.sharedDir = QDir::toNativeSeparators(QDir::currentPath() + QDir::separator());
	systemDirectories.docDir =
		QDir::toNativeSeparators(QDir::currentPath() + QDir::separator() + "doc" + QDir::separator());
#elif SHARED_DIR_IS_APP_DIR
	/* used for AppImage, which requires fixed data bundled at same location, as the application */
	QString sharePath;
#if defined(__APPLE__) || defined(__MACOSX)
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
	sharePath = QString::fromCFString(macPath);
	CFRelease(appUrlRef);
	CFRelease(macPath);
#else
	sharePath = QDir::currentPath();
#endif
	out << "sharePath directory: " << sharePath << endl;

	systemDirectories.sharedDir = QDir::toNativeSeparators(sharePath + QDir::separator());
	systemDirectories.docDir =
		QDir::toNativeSeparators(sharePath + QDir::separator() + "doc" + QDir::separator());
#else //_WIN32
// if SHARED_DIR is not defined under Linux then use path relative to main executable directory
#ifndef SHARED_DIR
	QDir shareDir(QCoreApplication::applicationDirPath());
	bool success = false;
	if (shareDir.cdUp())
	{
		if (shareDir.cd("share"))
		{
			QDir shareMandelbulber = shareDir;
			if (shareMandelbulber.cd("mandelbulber2"))
			{
				systemDirectories.sharedDir =
					QDir::cleanPath(shareMandelbulber.absolutePath()) + QDir::separator();
				success = true;
			}
			else
			{
				outErr << "Error! There is no 'mandelbulber2' directory in "
							 << shareMandelbulber.absolutePath() << endl;
			}

			QDir shareDocMandelbulber = shareDir;
			if (shareDocMandelbulber.cd("doc/mandelbulber2"))
			{
				systemDirectories.docDir =
					QDir::cleanPath(shareDocMandelbulber.absolutePath()) + QDir::separator();
			}
			else
			{
				outErr << "Error! There is no 'doc/mandelbulber2' directory in "
							 << shareDocMandelbulber.absolutePath() << endl;
			}
		}
		else
		{
			outErr << "Error! There is no 'share' directory in " << shareDir.absolutePath() << endl;
		}
	}
	else
	{
		outErr << "Error: " << QCoreApplication::applicationDirPath() << " has no parent directory!"
					 << endl;
	}

	// try to use default share dir
	if (!success)
	{
		outErr << "Trying to use /usr/share/mandelbulber2 as program data directory" << endl;
		if (shareDir.cd("/usr/share/mandelbulber2"))
		{
			systemDirectories.sharedDir = QDir::cleanPath(shareDir.absolutePath()) + QDir::separator();
		}
		else
		{
			outErr << "Error! Directory "
						 << "/usr/share/mandelbulber2"
						 << "doesn't exist!" << endl;
		}

		if (shareDir.cd("/usr/share/doc/mandelbulber2"))
		{
			systemDirectories.docDir = QDir::cleanPath(shareDir.absolutePath()) + QDir::separator();
		}
		else
		{
			outErr << "Error! Directory "
						 << "/usr/share/doc/mandelbulber2"
						 << "doesn't exist!" << endl;
		}
	}

#else	// SHARED_DIR
	systemDirectories.sharedDir = QDir::toNativeSeparators(QString(SHARED_DIR) + QDir::separator());
	systemDirectories.docDir = QDir::toNativeSeparators(QString(SHARED_DOC_DIR) + QDir::separator());
#endif // else SHARED_DIR
#endif // else _WIN32

// logfile
#ifdef _WIN32 /* WINDOWS */
	systemData.logfileName = systemDirectories.homeDir + "mandelbulber_log.txt";
#else
	systemData.logfileName = systemDirectories.homeDir + ".mandelbulber_log.txt";
#endif
	FILE *logfile = fopen(systemData.logfileName.toLocal8Bit().constData(), "w");
	if (logfile)
	{
		fclose(logfile);
	}

	out << "Mandelbulber " << MANDELBULBER_VERSION_STRING << "\n";
	out << "Log file name: " << systemData.logfileName << endl;
	WriteLogString("Mandelbulber version", QString(MANDELBULBER_VERSION_STRING), 1);

	// detecting number of CPU cores
	systemData.numberOfThreads = get_cpu_count();

	printf("Detected %d CPUs\n", systemData.numberOfThreads);
	WriteLogDouble("CPUs detected", systemData.numberOfThreads, 2);

#ifdef ONETHREAD // for debugging
	NR_THREADS = 1;
#endif

// data directory location
#ifdef _WIN32 /* WINDOWS */
	systemDirectories.SetDataDirectoryHidden(
		systemDirectories.homeDir + "mandelbulber" + QDir::separator());
	systemDirectories.SetDataDirectoryPublic(
		systemDirectories.homeDir + "mandelbulber" + QDir::separator());
#else
	systemDirectories.SetDataDirectoryHidden(
		QDir::toNativeSeparators(systemDirectories.homeDir + ".mandelbulber" + QDir::separator()));
	systemDirectories.SetDataDirectoryPublic(
		QDir::toNativeSeparators(systemDirectories.homeDir + "mandelbulber" + QDir::separator()));
#endif
	out << "Program data files directory " << systemDirectories.sharedDir << endl;
	out << "Default data hidden directory: " << systemDirectories.GetDataDirectoryHidden() << endl;
	WriteLogString("Default data hidden directory", systemDirectories.GetDataDirectoryHidden(), 1);
	out << "Default data public directory: " << systemDirectories.GetDataDirectoryPublic() << endl;
	WriteLogString("Default data public directory", systemDirectories.GetDataDirectoryPublic(), 1);

	//*********** temporary set to false ************
	systemData.noGui = false;
	systemData.silent = false;

	systemData.lastSettingsFile = QDir::toNativeSeparators(
		systemDirectories.GetSettingsFolder() + QDir::separator() + QString("settings.fract"));
	systemData.lastImageFile = QDir::toNativeSeparators(
		systemDirectories.GetImagesFolder() + QDir::separator() + QString("image.jpg"));
	systemData.lastImagePaletteFile = QDir::toNativeSeparators(
		systemDirectories.sharedDir + "textures" + QDir::separator() + QString("colour palette.jpg"));
	systemData.lastGradientFile = QDir::toNativeSeparators(
		systemDirectories.GetGradientsFolder() + QDir::separator() + "colors.gradient");

	QLocale systemLocale = QLocale::system();
	systemData.decimalPoint = systemLocale.decimalPoint();
	WriteLogString("Decimal point", QString(systemData.decimalPoint), 2);

	systemData.supportedLanguages.insert("en_EN", "English");
	systemData.supportedLanguages.insert("pl_PL", "Polski");
	systemData.supportedLanguages.insert("de_DE", "Deutsch");
	systemData.supportedLanguages.insert("it_IT", "Italiano");
	systemData.supportedLanguages.insert("nl_NL", "Nederlands");
	systemData.supportedLanguages.insert("es_ES", "Español");

	// get number of columns of console
	systemData.terminalWidth = 80;
	systemData.loggingVerbosity = 3;
	systemData.settingsLoadedFromCLI = false;
	systemData.globalStopRequest = false;
	systemData.isOutputTty = IsOutputTty();

#ifndef WIN32
	handle_winch(-1);
#endif
	return true;
}

// SIGWINCH is called when the window is resized.
void handle_winch(int sig)
{
	(void)sig;
#ifndef _WIN32
	signal(SIGWINCH, SIG_IGN);
	struct winsize w
	{
	};
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	systemData.terminalWidth = w.ws_col;
	if (systemData.terminalWidth <= 0) systemData.terminalWidth = 80;
	if (systemData.terminalWidth >= 150) systemData.terminalWidth = 150;
	if (!systemData.isOutputTty) systemData.terminalWidth = 80;
	signal(SIGWINCH, handle_winch);
#endif
}

int get_cpu_count()
{
	return QThread::idealThreadCount();
}

bool CreateDefaultFolders()
{
	// create data directory if not exists
	bool result = true;

	result &= CreateFolder(systemDirectories.GetDataDirectoryHidden());
	result &= CreateFolder(systemDirectories.GetDataDirectoryPublic());
	result &= CreateFolder(systemDirectories.GetImagesFolder());
	result &= CreateFolder(systemDirectories.GetThumbnailsFolder());
	result &= CreateFolder(systemDirectories.GetToolbarFolder());
	result &= CreateFolder(systemDirectories.GetHttpCacheFolder());
	result &= CreateFolder(systemDirectories.GetCustomWindowStateFolder());
	result &= CreateFolder(systemDirectories.GetSettingsFolder());
	result &= CreateFolder(systemDirectories.GetSlicesFolder());
	result &= CreateFolder(systemDirectories.GetMaterialsFolder());
	result &= CreateFolder(systemDirectories.GetAnimationFolder());
	result &= CreateFolder(systemDirectories.GetNetrenderFolder());
	result &= CreateFolder(systemDirectories.GetGradientsFolder());
	result &= CreateFolder(systemDirectories.GetOpenCLTempFolder());
	result &= CreateFolder(systemDirectories.GetOpenCLCustomFormulasFolder());
	result &= CreateFolder(systemDirectories.GetUndoFolder());
	result &= PutClangFormatFileToDataDirectoryHidden();

	RetrieveToolbarPresets(false);
	RetrieveExampleMaterials(false);

#ifdef CLSUPPORT
	string oclDir = systemData.dataDirectory + "/custom_ocl_formulas";
	QString qoclDir = QString::fromStdString(oclDir);
	if (!QDir(qoclDir).exists())
	{
		result &= CreateFolder(oclDir);
		if (result)
		{
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1.c", oclDir + "/cl_example1.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2.c", oclDir + "/cl_example2.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3.c", oclDir + +"/cl_example3.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1Init.c",
				oclDir + "/cl_example1Init.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2Init.c",
				oclDir + "/cl_example2Init.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3Init.c",
				oclDir + "/cl_example3Init.c");
		}
	}
#endif

	actualFileNames.actualFilenameSettings =
		QString("settings") + QDir::separator() + "default.fract";
	actualFileNames.actualFilenameSettings =
		QDir::toNativeSeparators(actualFileNames.actualFilenameSettings);

	actualFileNames.actualFilenameImage = QString("images") + QDir::separator() + "image.jpg";
	actualFileNames.actualFilenameImage =
		QDir::toNativeSeparators(actualFileNames.actualFilenameImage);

	actualFileNames.actualFilenamePalette =
		systemDirectories.sharedDir + "textures" + QDir::separator() + "colour palette.jpg";
	actualFileNames.actualFilenamePalette =
		QDir::toNativeSeparators(actualFileNames.actualFilenamePalette);

	ClearNetRenderCache();
	DeleteOldChache(systemDirectories.GetThumbnailsFolder(), 90);
	DeleteOldChache(systemDirectories.GetHttpCacheFolder(), 10);

	return result;
}

bool CreateFolder(const QString &qName)
{
	if (QDir(qName).exists())
	{
		WriteLogString("Directory already exists", qName, 1);
		return true;
	}
	else
	{
		if (QDir().mkdir(qName))
		{
			WriteLogString("Directory created", qName, 2);
			return true;
		}
		else
		{
			WriteLogString("Directory can't be created", qName, 1);
			qCritical() << "error: directory " << qName << " cannot be created" << endl;
			return false;
		}
	}
}

void DeleteAllFilesFromDirectory(
	const QString &folder, const QString &filterExpression, QRegExp::PatternSyntax pattern)
{
	QRegExp rx(filterExpression);
	rx.setPatternSyntax(pattern);

	if (QDir(folder).exists())
	{
		QDirIterator folderIterator(folder);
		while (folderIterator.hasNext())
		{
			folderIterator.next();
			if (folderIterator.fileName() == "." || folderIterator.fileName() == "..") continue;
			if (rx.exactMatch(folderIterator.fileName()))
			{
				if (QFile::remove(folderIterator.filePath()))
				{
					WriteLogString("File deleted", folderIterator.filePath(), 2);
				}
				else
				{
					WriteLogString("File not deleted", folderIterator.filePath(), 1);
				}
			}
		}
	}
	else
	{
		WriteLogString("Directory does not exist", folder, 2);
	}
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	QString text;
	switch (type)
	{
#if QT_VERSION >= 0x050500
		case QtInfoMsg:
			fprintf(stderr, "Info: %s\n", localMsg.constData());
			text = QString("Info: ") + QString(localMsg.constData());
			break;
#endif
		case QtDebugMsg:
			fprintf(stderr, "Debug: %s\n", localMsg.constData());
			text = QString("Debug: ") + QString(localMsg.constData());
			break;
		case QtWarningMsg:
			fprintf(stderr, "Warning: %s\n(%s:%u, %s)\n\n", localMsg.constData(), context.file,
				context.line, context.function);
			text = QString("Warning: ") + QString(localMsg.constData()) + " (" + context.file + ":"
						 + QString::number(context.line) + ", " + context.function;
			break;
		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s\n(%s:%u, %s)\n\n", localMsg.constData(), context.file,
				context.line, context.function);
			text = QString("Critical: ") + QString(localMsg.constData()) + " (" + context.file + ":"
						 + QString::number(context.line) + ", " + context.function;
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n(%s:%u, %s)\n\n", localMsg.constData(), context.file,
				context.line, context.function);
			text = QString("Fatal: ") + QString(localMsg.constData()) + " (" + context.file + ":"
						 + QString::number(context.line) + ", " + context.function;
			abort();
			break;
	}
	WriteLog(text, 1);

	if (type == QtFatalMsg) abort();
}

void UpdateDefaultPaths()
{
	systemData.lastSettingsFile =
		gPar->Get<QString>("default_settings_path") + QDir::separator() + QString("settings.fract");
	systemData.lastImageFile =
		gPar->Get<QString>("default_image_path") + QDir::separator() + QString("image.jpg");
	gPar->Set("file_background",
		gPar->Get<QString>("default_textures_path") + QDir::separator() + "background.jpg");
	gPar->Set(
		"file_envmap", gPar->Get<QString>("default_textures_path") + QDir::separator() + "envmap.jpg");
	gPar->Set("file_lightmap",
		gPar->Get<QString>("default_textures_path") + QDir::separator() + "lightmap.jpg");
}

void UpdateUIStyle()
{
	// Selecting default Fusion UI style in case if wrong style is selected
	if (gPar->Get<int>("ui_style_type") < 0
			|| gPar->Get<int>("ui_style_type") >= QStyleFactory::keys().size())
	{
		QStringList listOfStyles = QStyleFactory::keys();
		int indexOfFusion = listOfStyles.indexOf("Fusion");
		gPar->Set<int>("ui_style_type", indexOfFusion);
	}

	// set ui style
	if (gPar->Get<int>("ui_style_type") >= 0
			&& gPar->Get<int>("ui_style_type") < QStyleFactory::keys().size())
	{
		QApplication::setStyle(
			QStyleFactory::create(QStyleFactory::keys().at(gPar->Get<int>("ui_style_type"))));
	}
}

void UpdateUISkin()
{
	static QPalette defaultPalette; // cache "normal" skin on first call
	QPalette palette;

	QColor colorBackground1 = palette.color(QPalette::Window);
	QColor colorBackground2 = palette.color(QPalette::Base);
	QColor colorText1 = palette.color(QPalette::WindowText);
	QColor colorText2 = palette.color(QPalette::HighlightedText);

	switch (gPar->Get<int>("ui_skin"))
	{
		case 1: // dark skin
			colorBackground1 = QColor(53, 53, 53);
			colorBackground2 = QColor(25, 25, 25);
			colorText1 = Qt::white;
			colorText2 = Qt::black;
			break;
		case 2: // light skin (only roughly inverted dark skin)
			colorBackground1 = QColor(240, 240, 240);
			colorBackground2 = QColor(250, 250, 250);
			colorText1 = Qt::black;
			colorText2 = Qt::white;
			break;
		case 3: // Nasa Font light
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			QApplication::setFont(QFont("nasalization"));
			colorBackground1 = QColor(167, 173, 187);
			colorBackground2 = QColor(192, 196, 207);
			colorText1 = Qt::black;
			colorText2 = Qt::white;
			break;
		case 4: // Nasa Font dark
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			QApplication::setFont(QFont("nasalization"));
			colorBackground1 = QColor(52, 61, 70);
			colorBackground2 = QColor(79, 91, 102);
			colorText1 = Qt::white;
			colorText2 = Qt::black;
			break;
		case 5: // Nasa Font dark green
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			QApplication::setFont(QFont("nasalization"));
			colorBackground1 = QColor(20, 40, 10);
			colorBackground2 = QColor(30, 50, 0);
			colorText1 = Qt::green;
			colorText2 = Qt::black;
			break;
		case 6: // Nasa Font dark blue
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			QApplication::setFont(QFont("nasalization"));
			colorBackground1 = QColor(10, 10, 40);
			colorBackground2 = QColor(20, 20, 60);
			colorText1 = QColor(50, 150, 255);
			colorText2 = Qt::black;
			break;
		case 7: // Nasa Font dark brown
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			QApplication::setFont(QFont("nasalization"));
			colorBackground1 = QColor(40, 10, 10);
			colorBackground2 = QColor(60, 20, 20);
			colorText1 = QColor(255, 150, 50);
			colorText2 = Qt::black;
			break;
		default: // normal skin
			palette = defaultPalette;
			break;
	}
	if (gPar->Get<int>("ui_skin") != 0)
	{
		palette.setColor(QPalette::Window, colorBackground1);
		palette.setColor(QPalette::WindowText, colorText1);
		palette.setColor(QPalette::Base, colorBackground2);
		palette.setColor(QPalette::AlternateBase, colorBackground1);
		palette.setColor(QPalette::ToolTipBase, colorBackground2);
		palette.setColor(QPalette::ToolTipText, colorText1);
		palette.setColor(QPalette::Text, colorText1);
		palette.setColor(QPalette::Button, colorBackground1);
		palette.setColor(QPalette::ButtonText, colorText1);
		palette.setColor(QPalette::BrightText, Qt::red);
		palette.setColor(QPalette::Link, QColor(42, 130, 218));

		palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
		palette.setColor(QPalette::HighlightedText, colorText2);
	}
	// set ui skin
	QApplication::setPalette(palette);
}

void UpdateLanguage()
{
	// Set language from locale
	WriteLog("Prepare translator", 2);
	static QTranslator mandelbulberMainTranslator;
	static QTranslator mandelbulberFractalUiTranslator;
	static QTranslator qtTranslator;

	QString locale = systemData.locale.name();
	if (gPar)
	{
		if (systemData.supportedLanguages.contains(gPar->Get<QString>("language")))
		{
			locale = gPar->Get<QString>("language");
		}
	}

	WriteLogString("locale", locale, 2);
	mandelbulberMainTranslator.load(
		locale, systemDirectories.sharedDir + QDir::separator() + "language");
	mandelbulberFractalUiTranslator.load(
		"formula_" + locale, systemDirectories.sharedDir + QDir::separator() + "language");

	WriteLog("Installing translator", 2);
	QCoreApplication::installTranslator(&mandelbulberMainTranslator);
	QCoreApplication::installTranslator(&mandelbulberFractalUiTranslator);

	// try to load qt translator
	if (qtTranslator.load(
				QLatin1String("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath))
			|| qtTranslator.load(QLatin1String("qtbase_") + locale,
					 QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
	{
		QCoreApplication::installTranslator(&qtTranslator);
	}
}

void RetrieveToolbarPresets(bool force)
{
	if (QDir(systemDirectories.GetToolbarFolder())
					.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)
					.count()
				== 0
			|| force)
	{
		// first run of program (or all toolbar items deleted) -> copy toolbar presets to working folder
		QDirIterator toolbarFiles(systemDirectories.sharedDir + "toolbar");
		while (toolbarFiles.hasNext())
		{
			toolbarFiles.next();
			if (toolbarFiles.fileName() == "." || toolbarFiles.fileName() == "..") continue;
			fcopy(toolbarFiles.filePath(),
				systemDirectories.GetToolbarFolder() + QDir::separator() + toolbarFiles.fileName());
		}
	}
}

void RetrieveExampleMaterials(bool force)
{
	if (QDir(systemDirectories.GetMaterialsFolder())
					.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)
					.count()
				== 0
			|| force)
	{
		// first run of program (or all material items deleted) -> copy materials to working folder
		QDirIterator materialFiles(systemDirectories.sharedDir + "materials");
		while (materialFiles.hasNext())
		{
			materialFiles.next();
			if (materialFiles.fileName() == "." || materialFiles.fileName() == "..") continue;
			fcopy(materialFiles.filePath(),
				systemDirectories.GetMaterialsFolder() + QDir::separator() + materialFiles.fileName());
		}
	}
}

void CalcPreferredFontSize(bool noGui)
{
	if (!noGui)
	{
		const int fontSize = (int)QApplication::font().pointSizeF();
		systemData.SetPreferredFontPointSize(fontSize);

		QFontMetrics fm(QApplication::font());
		const int pixelFontSize = fm.height();

		const int thumbnailSize = (pixelFontSize * 8);
		systemData.SetPreferredThumbnailSize(thumbnailSize);
	}
	else
	{
		systemData.SetPreferredFontPointSize(10);
		systemData.SetPreferredFontSize(10);
		systemData.SetPreferredThumbnailSize(80);
	}
}

bool IsOutputTty()
{
#ifdef _WIN32 /* WINDOWS */
	return false;
#else
	return isatty(fileno(stdout));
#endif
}

void ClearNetRenderCache()
{
	QDir dir(systemDirectories.GetNetrenderFolder());
	if (dir.exists())
	{
		QDirIterator it(dir);
		while (it.hasNext())
		{
			QFile file(it.next());
			file.remove();
		}
	}
}

void DeleteOldChache(const QString &directoryPath, int maxDays)
{
	QDir dir(directoryPath);
	int counter = 0;
	qint64 totalSize = 0;
	if (dir.exists())
	{
		QDirIterator it(dir);
		while (it.hasNext())
		{
			QFile file(it.next());
			QFileInfo fileInfo(file);
			QDateTime dateTime = fileInfo.lastModified();
			qint64 days = dateTime.daysTo(QDateTime::currentDateTime());
			if (days > maxDays)
			{
				totalSize += file.size();
				file.remove();
				counter++;
			}
		}
	}
	if (counter > 0)
	{
		QString message = QString("Removed %1 old files from %2. Saved %3 MB")
												.arg(counter)
												.arg(directoryPath)
												.arg(totalSize / 1024 / 1024);
		qInfo() << message;
		WriteLog(message, 2);
	}
}

bool PutClangFormatFileToDataDirectoryHidden()
{
	QFile file(systemDirectories.GetDataDirectoryHidden() + "/.clang-format");
	if (file.exists()) return true;
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		out << "---\n"
					 "Language:        Cpp\n"
					 "# BasedOnStyle:  LLVM\n"
					 "AccessModifierOffset: -2\n"
					 "AlignAfterOpenBracket: false\n"
					 "AlignEscapedNewlinesLeft: true\n"
					 "AlignTrailingComments: true\n"
					 "AlignOperands:   true\n"
					 "AllowAllParametersOfDeclarationOnNextLine: true\n"
					 "AllowShortBlocksOnASingleLine: false\n"
					 "AllowShortCaseLabelsOnASingleLine: true\n"
					 "AllowShortIfStatementsOnASingleLine: true\n"
					 "AllowShortLoopsOnASingleLine: false\n"
					 "AllowShortFunctionsOnASingleLine: Inline\n"
					 "AlwaysBreakAfterDefinitionReturnType: false\n"
					 "AlwaysBreakTemplateDeclarations: true\n"
					 "AlwaysBreakBeforeMultilineStrings: true\n"
					 "BreakBeforeBinaryOperators: NonAssignment\n"
					 "BreakBeforeTernaryOperators: true\n"
					 "BreakConstructorInitializersBeforeComma: false\n"
					 "BinPackParameters: true\n"
					 "BinPackArguments: true\n"
					 "ColumnLimit:     100\n"
					 "ConstructorInitializerAllOnOneLineOrOnePerLine: true\n"
					 "ConstructorInitializerIndentWidth: 4\n"
					 "DerivePointerAlignment: false\n"
					 "ExperimentalAutoDetectBinPacking: false\n"
					 "IndentCaseLabels: true\n"
					 "IndentWrappedFunctionNames: false\n"
					 "IndentFunctionDeclarationAfterType: false\n"
					 "MaxEmptyLinesToKeep: 1\n"
					 "KeepEmptyLinesAtTheStartOfBlocks: true\n"
					 "NamespaceIndentation: None\n"
					 "ObjCBlockIndentWidth: 2\n"
					 "ObjCSpaceAfterProperty: false\n"
					 "ObjCSpaceBeforeProtocolList: true\n"
					 "PenaltyBreakBeforeFirstCallParameter: 19\n"
					 "PenaltyBreakComment: 300\n"
					 "PenaltyBreakString: 1000\n"
					 "PenaltyBreakFirstLessLess: 120\n"
					 "PenaltyExcessCharacter: 1000000\n"
					 "PenaltyReturnTypeOnItsOwnLine: 60\n"
					 "PointerAlignment: Right\n"
					 "SpacesBeforeTrailingComments: 1\n"
					 "Cpp11BracedListStyle: true\n"
					 "Standard:        Cpp11\n"
					 "IndentWidth:     2\n"
					 "TabWidth:        2\n"
					 "UseTab:          Always\n"
					 "BreakBeforeBraces: Allman\n"
					 "SpacesInParentheses: false\n"
					 "SpacesInSquareBrackets: false\n"
					 "SpacesInAngles:  false\n"
					 "SpaceInEmptyParentheses: false\n"
					 "SpacesInCStyleCastParentheses: false\n"
					 "SpaceAfterCStyleCast: false\n"
					 "SpacesInContainerLiterals: true\n"
					 "SpaceBeforeAssignmentOperators: true\n"
					 "ContinuationIndentWidth: 2\n"
					 "CommentPragmas:  '^ IWYU pragma:'\n"
					 "ForEachMacros:   [ foreach, Q_FOREACH, BOOST_FOREACH ]\n"
					 "SpaceBeforeParens: ControlStatements\n"
					 "DisableFormat:   false\n"
					 "SortIncludes: false\n"
					 "...\n";
		file.close();
		return true;
	}
	return false;
}
