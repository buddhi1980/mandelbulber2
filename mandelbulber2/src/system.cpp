/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "global_data.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include <QTextStream>
#include <QtGui>
#include <ctime>
#include <locale.h>
#include <qstylefactory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/render_window.hpp"

#ifndef WIN32
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <stdio.h>

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

	systemData.homedir = QDir::toNativeSeparators(QDir::homePath() + QDir::separator());

#ifdef WIN32 /* WINDOWS */
	systemData.sharedDir = QDir::toNativeSeparators(QDir::currentPath() + QDir::separator());
#else
	systemData.sharedDir = QDir::toNativeSeparators(QString(SHARED_DIR) + QDir::separator());
#endif /* WINDOWS */

// logfile
#ifdef WIN32 /* WINDOWS */
	systemData.logfileName = systemData.homedir + "mandelbulber_log.txt";
#else
	systemData.logfileName = systemData.homedir + ".mandelbulber_log.txt";
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
#ifdef WIN32 /* WINDOWS */
	systemData.SetDataDirectoryHidden(systemData.homedir + "mandelbulber" + QDir::separator());
	systemData.SetDataDirectoryPublic(systemData.homedir + "mandelbulber" + QDir::separator());
#else
	systemData.SetDataDirectoryHidden(
		QDir::toNativeSeparators(systemData.homedir + ".mandelbulber" + QDir::separator()));
	systemData.SetDataDirectoryPublic(
		QDir::toNativeSeparators(systemData.homedir + "mandelbulber" + QDir::separator()));
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
#ifndef WIN32
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

void WriteLog(QString text, int verbosityLevel)
{
	if (verbosityLevel <= systemData.loggingVerbosity)
	{
		FILE *logfile = fopen(systemData.logfileName.toLocal8Bit().constData(), "a");
#ifdef WIN32
		QString logtext = QString("PID: %1, time: %2, %3\n")
												.arg(QCoreApplication::applicationPid())
												.arg(QString::number(clock() / 1.0e3, 'f', 3))
												.arg(text);
#else
		QString logtext =
			QString("PID: %1, time: %2, %3\n")
				.arg(QCoreApplication::applicationPid())
				.arg(QString::number((systemData.globalTimer.nsecsElapsed()) / 1.0e9, 'f', 9))
				.arg(text);
#endif

		fputs(logtext.toLocal8Bit().constData(), logfile);
		fclose(logfile);

		// write to log in window
		if (gMainInterface && gMainInterface->mainWindow != nullptr)
		{
			gMainInterface->mainWindow->AppendToLog(logtext);
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

bool CreateDefaultFolders()
{
	// create data directory if not exists
	bool result = true;

	result &= CreateFolder(systemData.GetDataDirectoryHidden());
	result &= CreateFolder(systemData.GetDataDirectoryPublic());
	result &= CreateFolder(systemData.GetImagesFolder());
	result &= CreateFolder(systemData.GetThumbnailsFolder());
	result &= CreateFolder(systemData.GetToolbarFolder());
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

bool CreateFolder(QString qname)
{
	if (QDir(qname).exists())
	{
		WriteLogString("Directory already exists", qname, 1);
		return true;
	}
	else
	{
		if (QDir().mkdir(qname))
		{
			WriteLogString("Directory created", qname, 2);
			return true;
		}
		else
		{
			WriteLogString("Directory can't be created", qname, 1);
			qCritical() << "error: directory " << qname << " cannot be created" << endl;
			return false;
		}
	}
}

void DeleteAllFilesFromDirectory(QString folder, QString filterExpression)
{
	QRegExp rx(filterExpression);
	rx.setPatternSyntax(QRegExp::Wildcard);

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

	FILE *pFile;
	long int lSize;
	char *buffer;
	size_t result;

	pFile = fopen(source.toLocal8Bit().constData(), "rb");
	if (pFile == nullptr)
	{
		qCritical() << "Can't open source file for copying: " << source << endl;
		WriteLogString("Can't open source file for copying", source, 1);
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	if (lSize > 0)
	{
		buffer = new char[lSize];

		// copy the file into the buffer:
		result = fread(buffer, 1, lSize, pFile);
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

	switch (gPar->Get<int>("ui_skin"))
	{
		case 1: // dark skin
			palette.setColor(QPalette::Window, QColor(53, 53, 53));
			palette.setColor(QPalette::WindowText, Qt::white);
			palette.setColor(QPalette::Base, QColor(25, 25, 25));
			palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
			palette.setColor(QPalette::ToolTipBase, Qt::white);
			palette.setColor(QPalette::ToolTipText, Qt::white);
			palette.setColor(QPalette::Text, Qt::white);
			palette.setColor(QPalette::Button, QColor(53, 53, 53));
			palette.setColor(QPalette::ButtonText, Qt::white);
			palette.setColor(QPalette::BrightText, Qt::red);
			palette.setColor(QPalette::Link, QColor(42, 130, 218));

			palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
			palette.setColor(QPalette::HighlightedText, Qt::black);
			palette.setColor(QPalette::ToolTipBase, Qt::black);
			palette.setColor(QPalette::ToolTipText, QColor(42, 130, 218));
			break;
		case 2: // light skin (only roughly inverted dark skin)
			palette.setColor(QPalette::Window, QColor(240, 240, 240));
			palette.setColor(QPalette::WindowText, Qt::black);
			palette.setColor(QPalette::Base, QColor(250, 250, 250));
			palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
			palette.setColor(QPalette::ToolTipBase, Qt::black);
			palette.setColor(QPalette::ToolTipText, Qt::black);
			palette.setColor(QPalette::Text, Qt::black);
			palette.setColor(QPalette::Button, QColor(240, 240, 240));
			palette.setColor(QPalette::ButtonText, Qt::black);
			palette.setColor(QPalette::BrightText, Qt::red);
			palette.setColor(QPalette::Link, QColor(42, 130, 218));

			palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
			palette.setColor(QPalette::HighlightedText, Qt::white);
			palette.setColor(QPalette::ToolTipBase, Qt::white);
			palette.setColor(QPalette::ToolTipText, QColor(42, 130, 218));
			break;
		default: // normal skin
			palette = defaultPalette;
			break;
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
		"qt_data_" + locale, systemData.sharedDir + QDir::separator() + "language");

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
