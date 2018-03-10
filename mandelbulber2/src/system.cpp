/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <locale.h>
#include <qstylefactory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctime>
#include <iostream>

#include <QTextStream>
#include <QtGui>

#include "global_data.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include "render_window.hpp"

// custom includes
#ifndef _WIN32
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
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

	systemData.globalTimer.start();

	systemData.homeDir = QDir::toNativeSeparators(QDir::homePath() + QDir::separator());
#ifdef _WIN32 /* WINDOWS */
	systemData.sharedDir = QDir::toNativeSeparators(QDir::currentPath() + QDir::separator());
	systemData.docDir =
		QDir::toNativeSeparators(QDir::currentPath() + QDir::separator() + "doc" + QDir::separator());
#elif SHARED_DIR_IS_APP_DIR
	/* used for AppImage, which requires fixed data bundled at same location, as the application */
	systemData.sharedDir = QDir::toNativeSeparators(QDir::currentPath() + QDir::separator());
	systemData.docDir =
		QDir::toNativeSeparators(QDir::currentPath() + QDir::separator() + "doc" + QDir::separator());

#else
	systemData.sharedDir = QDir::toNativeSeparators(QString(SHARED_DIR) + QDir::separator());
	systemData.docDir = QDir::toNativeSeparators(QString(SHARED_DOC_DIR) + QDir::separator());
#endif

// logfile
#ifdef _WIN32 /* WINDOWS */
	systemData.logfileName = systemData.homeDir + "mandelbulber_log.txt";
#else
	systemData.logfileName = systemData.homeDir + ".mandelbulber_log.txt";
#endif
	FILE *logfile = fopen(systemData.logfileName.toLocal8Bit().constData(), "w");
	fclose(logfile);

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
	systemData.SetDataDirectoryHidden(systemData.homeDir + "mandelbulber" + QDir::separator());
	systemData.SetDataDirectoryPublic(systemData.homeDir + "mandelbulber" + QDir::separator());
#else
	systemData.SetDataDirectoryHidden(
		QDir::toNativeSeparators(systemData.homeDir + ".mandelbulber" + QDir::separator()));
	systemData.SetDataDirectoryPublic(
		QDir::toNativeSeparators(systemData.homeDir + "mandelbulber" + QDir::separator()));
#endif
	out << "Default data hidden directory: " << systemData.GetDataDirectoryHidden() << endl;
	WriteLogString("Default data hidden directory", systemData.GetDataDirectoryHidden(), 1);
	out << "Default data public directory: " << systemData.GetDataDirectoryPublic() << endl;
	WriteLogString("Default data public directory", systemData.GetDataDirectoryPublic(), 1);

	//*********** temporary set to false ************
	systemData.noGui = false;
	systemData.silent = false;

	systemData.lastSettingsFile = QDir::toNativeSeparators(
		systemData.GetSettingsFolder() + QDir::separator() + QString("settings.fract"));
	systemData.lastImageFile = QDir::toNativeSeparators(
		systemData.GetImagesFolder() + QDir::separator() + QString("image.jpg"));
	systemData.lastImagePaletteFile = QDir::toNativeSeparators(
		systemData.sharedDir + "textures" + QDir::separator() + QString("colour palette.jpg"));

	QLocale systemLocale = QLocale::system();
	systemData.decimalPoint = systemLocale.decimalPoint();
	WriteLogString("Decimal point", QString(systemData.decimalPoint), 2);

	systemData.supportedLanguages.insert("en_EN", "English");
	systemData.supportedLanguages.insert("pl_PL", "Polski");
	systemData.supportedLanguages.insert("de_DE", "Deutsch");
	systemData.supportedLanguages.insert("it_IT", "Italiano");
	systemData.supportedLanguages.insert("nl_NL", "Nederlands");

	// get number of columns of console
	systemData.terminalWidth = 80;

	systemData.loggingVerbosity = 3;

	systemData.settingsLoadedFromCLI = false;

	systemData.globalStopRequest = false;

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
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	systemData.terminalWidth = w.ws_col;
	if (systemData.terminalWidth <= 0) systemData.terminalWidth = 80;
	signal(SIGWINCH, handle_winch);
#endif
}

int get_cpu_count()
{
	return QThread::idealThreadCount();
}

void WriteLogCout(QString text, int verbosityLevel)
{
	// output to console
	cout << text.toStdString();
	cout.flush();
	WriteLog(text, verbosityLevel);
}

void WriteLog(QString text, int verbosityLevel)
{
	// verbosity level:
	// 1 - only errors
	// 2 - main events / actions
	// 3 - detailed events / actions

	if (verbosityLevel <= systemData.loggingVerbosity)
	{
		FILE *logfile = fopen(systemData.logfileName.toLocal8Bit().constData(), "a");
#ifdef _WIN32
		QString logText = QString("PID: %1, time: %2, %3\n")
												.arg(QCoreApplication::applicationPid())
												.arg(QString::number(clock() / 1.0e3, 'f', 3))
												.arg(text);
#else
		QString logText =
			QString("PID: %1, time: %2, %3\n")
				.arg(QCoreApplication::applicationPid())
				.arg(QString::number((systemData.globalTimer.nsecsElapsed()) / 1.0e9, 'f', 9))
				.arg(text);
#endif

		fputs(logText.toLocal8Bit().constData(), logfile);
		fclose(logfile);

		// write to log in window
		if (gMainInterface && gMainInterface->mainWindow != nullptr)
		{
			gMainInterface->mainWindow->AppendToLog(logText);
		}
	}
}

void WriteLogString(QString text, QString value, int verbosityLevel)
{
	WriteLog(text + ", value = " + value, verbosityLevel);
}

void WriteLogDouble(QString text, double value, int verbosityLevel)
{
	WriteLog(text + ", value = " + QString::number(value), verbosityLevel);
}

void WriteLogInt(QString text, int value, int verbosityLevel)
{
	WriteLog(text + ", value = " + QString::number(value), verbosityLevel);
}

void WriteLogSizeT(QString text, size_t value, int verbosityLevel)
{
	WriteLog(text + ", value = " + QString::number(value), verbosityLevel);
}

bool CreateDefaultFolders()
{
	// create data directory if not exists
	bool result = true;

	result &= CreateFolder(systemData.GetDataDirectoryHidden());
	result &= CreateFolder(systemData.GetDataDirectoryPublic());
	result &= CreateFolder(systemData.GetImagesFolder());
	result &= CreateFolder(systemData.GetThumbnailsFolder());
	result &= CreateFolder(systemData.GetToolbarFolder());
	result &= CreateFolder(systemData.GetHttpCacheFolder());
	result &= CreateFolder(systemData.GetCustomWindowStateFolder());
	result &= CreateFolder(systemData.GetSettingsFolder());
	result &= CreateFolder(systemData.GetSlicesFolder());
	result &= CreateFolder(systemData.GetMaterialsFolder());
	result &= CreateFolder(systemData.GetAnimationFolder());

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
		systemData.sharedDir + "textures" + QDir::separator() + "colour palette.jpg";
	actualFileNames.actualFilenamePalette =
		QDir::toNativeSeparators(actualFileNames.actualFilenamePalette);

	return result;
}

bool CreateFolder(QString qName)
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
	QString folder, QString filterExpression, QRegExp::PatternSyntax pattern)
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
		WriteLogString("Directory does not exist", folder, 1);
	}
}

int fcopy(QString source, QString dest)
{
	// ------ file reading

	char *buffer;

	FILE *pFile = fopen(source.toLocal8Bit().constData(), "rb");
	if (pFile == nullptr)
	{
		qCritical() << "Can't open source file for copying: " << source << endl;
		WriteLogString("Can't open source file for copying", source, 1);
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	const long int lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	if (lSize > 0)
	{
		buffer = new char[lSize];

		// copy the file into the buffer:
		const size_t result = fread(buffer, 1, lSize, pFile);
		if (result != size_t(lSize))
		{
			qCritical() << "Can't read source file for copying: " << source << endl;
			WriteLogString("Can't read source file for copying", source, 1);
			delete[] buffer;
			fclose(pFile);
			return 2;
		}
	}
	else
	{
		qCritical() << "Can't obtain file size: " << source;
		fclose(pFile);
		return 4;
	}
	fclose(pFile);

	// ----- file writing

	pFile = fopen(dest.toLocal8Bit().constData(), "wb");
	if (pFile == nullptr)
	{
		qCritical() << "Can't open destination file for copying: " << dest << endl;
		WriteLogString("Can't open destination file for copying", dest, 1);
		delete[] buffer;
		return 3;
	}
	fwrite(buffer, 1, lSize, pFile);
	fclose(pFile);

	delete[] buffer;
	WriteLogString("File copied", dest, 2);
	return 0;
}

void Wait(long int time)
{
	QMutex dummy;
	dummy.lock();
	QWaitCondition waitCondition;
	waitCondition.wait(&dummy, time);
	dummy.unlock();
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
	// set ui style
	if (gPar->Get<int>("ui_style_type") >= 0
			&& gPar->Get<int>("ui_style_type") < QStyleFactory::keys().size())
	{
		gApplication->setStyle(
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
			gApplication->setFont(QFont("nasalization"));
			colorBackground1 = QColor(167, 173, 187);
			colorBackground2 = QColor(192, 196, 207);
			colorText1 = Qt::black;
			colorText2 = Qt::white;
			break;
		case 4: // Nasa Font dark
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			gApplication->setFont(QFont("nasalization"));
			colorBackground1 = QColor(52, 61, 70);
			colorBackground2 = QColor(79, 91, 102);
			colorText1 = Qt::white;
			colorText2 = Qt::black;
			break;
		case 5: // Nasa Font dark green
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			gApplication->setFont(QFont("nasalization"));
			colorBackground1 = QColor(20, 40, 10);
			colorBackground2 = QColor(30, 50, 0);
			colorText1 = Qt::green;
			colorText2 = Qt::black;
			break;
		case 6: // Nasa Font dark blue
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			gApplication->setFont(QFont("nasalization"));
			colorBackground1 = QColor(10, 10, 40);
			colorBackground2 = QColor(20, 20, 60);
			colorText1 = QColor(50, 150, 255);
			colorText2 = Qt::black;
			break;
		case 7: // Nasa Font dark brown
			QFontDatabase::addApplicationFont(":/fonts/fonts/nasalization-rg.ttf");
			gApplication->setFont(QFont("nasalization"));
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
	gApplication->setPalette(palette);
}

void UpdateLanguage(QCoreApplication *app)
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
	mandelbulberMainTranslator.load(locale, systemData.sharedDir + QDir::separator() + "language");
	mandelbulberFractalUiTranslator.load(
		"formula_" + locale, systemData.sharedDir + QDir::separator() + "language");

	WriteLog("Installing translator", 2);
	app->installTranslator(&mandelbulberMainTranslator);
	app->installTranslator(&mandelbulberFractalUiTranslator);

	// try to load qt translator
	if (qtTranslator.load(
				QLatin1String("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath))
			|| qtTranslator.load(QLatin1String("qtbase_") + locale,
					 QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
	{
		app->installTranslator(&qtTranslator);
	}
}

void RetrieveToolbarPresets(bool force)
{
	if (QDir(systemData.GetToolbarFolder())
					.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)
					.count()
				== 0
			|| force)
	{
		// first run of program (or all toolbar items deleted) -> copy toolbar presets to working folder
		QDirIterator toolbarFiles(systemData.sharedDir + "toolbar");
		while (toolbarFiles.hasNext())
		{
			toolbarFiles.next();
			if (toolbarFiles.fileName() == "." || toolbarFiles.fileName() == "..") continue;
			fcopy(toolbarFiles.filePath(),
				systemData.GetToolbarFolder() + QDir::separator() + toolbarFiles.fileName());
		}
	}
}

void RetrieveExampleMaterials(bool force)
{
	if (QDir(systemData.GetMaterialsFolder())
					.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)
					.count()
				== 0
			|| force)
	{
		// first run of program (or all material items deleted) -> copy materials to working folder
		QDirIterator materialFiles(systemData.sharedDir + "materials");
		while (materialFiles.hasNext())
		{
			materialFiles.next();
			if (materialFiles.fileName() == "." || materialFiles.fileName() == "..") continue;
			fcopy(materialFiles.filePath(),
				systemData.GetMaterialsFolder() + QDir::separator() + materialFiles.fileName());
		}
	}
}

QThread::Priority GetQThreadPriority(enumRenderingThreadPriority priority)
{
	switch (priority)
	{
		case renderingPriorityLowest: return QThread::LowestPriority;
		case renderingPriorityLow: return QThread::LowPriority;
		case renderingPriorityNormal: return QThread::NormalPriority;
		case renderingPriorityHigh: return QThread::HighPriority;
		default: return QThread::NormalPriority;
	}
}

void CalcPreferredFontSize(bool noGui)
{
	if (!noGui)
	{
		const int fontSize = gApplication->font().pointSizeF();
		systemData.SetPreferredFontPointSize(fontSize);

		QFontMetrics fm(gApplication->font());
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

QString sSystem::GetIniFile() const
{
	double version = MANDELBULBER_VERSION;
	int versionInt = int(version * 100);

	QString iniFileName = QString("mandelbulber_%1.ini").arg(versionInt);
	QString fullIniFileName = dataDirectoryHidden + iniFileName;

	// if setting file doesn't exist then look for older files
	if (!QFile::exists(fullIniFileName))
	{
		QString tempFileName;
		for (int ver = versionInt; ver >= 212; ver--)
		{
			if (ver == 212)
			{
				tempFileName = QString("mandelbulber.ini");
			}
			else
			{
				tempFileName = QString("mandelbulber_%1.ini").arg(ver);
			}

			if (QFile::exists(dataDirectoryHidden + tempFileName))
			{
				fcopy(dataDirectoryHidden + tempFileName, fullIniFileName);
				WriteLogString("Found older settings file", dataDirectoryHidden + tempFileName, 1);
			}
		}
	}
	return fullIniFileName;
}
