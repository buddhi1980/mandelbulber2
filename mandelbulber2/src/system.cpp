/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * system data - place for system functions and definition of default file paths
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "system.hpp"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QtGui>
#include <ctime>
#include <QTextStream>
#include "global_data.hpp"
#include "settings.hpp"

//#define CLSUPPORT

sSystem systemData;
sActualFileNames actualFileNames;

bool InitSystem(void)
{
	setlocale(LC_ALL, "");
	systemData.locale = QLocale::system();
	systemData.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	QLocale::setDefault(systemData.locale);

	QTextStream out(stdout);

	systemData.homedir = QDir::homePath() + QDir::separator();

#ifdef WIN32 /* WINDOWS */
  systemData.sharedDir = (QDir::currentPath() + QDir::separator());
#else
	systemData.sharedDir = QString(SHARED_DIR) + QDir::separator();
#endif  /* WINDOWS */

	//logfile
#ifdef WIN32 /* WINDOWS */
	systemData.logfileName = systemData.homedir + "mandelbulber_log.txt";
#else
	systemData.logfileName = systemData.homedir + ".mandelbulber_log.txt";
#endif
	FILE *logfile = fopen(systemData.logfileName.toUtf8().constData(), "w");
	fclose(logfile);

	out << "Log file name: " << systemData.logfileName << endl;
	WriteLogString("Mandelbulber version", QString(MANDELBULBER_VERSION_STRING));
	WriteLogString("Mandelbulber compilation date", QString(__DATE__) + " " + QString(__TIME__));

	//detecting number of CPU cores
	systemData.numberOfThreads = get_cpu_count();
	//NR_THREADS = 1;

	printf("Detected %d CPUs\n", systemData.numberOfThreads);
	WriteLogDouble("CPUs detected", systemData.numberOfThreads);

#ifdef ONETHREAD //for debugging
	NR_THREADS = 1;
#endif

	//data directory location
#ifdef WIN32 /* WINDOWS */
	systemData.dataDirectory = systemData.homedir  + "mandelbulber" + QDir::separator();
#else
	systemData.dataDirectory = systemData.homedir  + ".mandelbulber" + QDir::separator();
#endif
	out << "Default data directory: " << systemData.dataDirectory << endl;
	WriteLogString("Default data directory", systemData.dataDirectory);

	systemData.thumbnailDir = systemData.dataDirectory + "thumbnails" + QDir::separator();

	systemData.autosaveFile = systemData.dataDirectory + ".autosave.fract";

	//*********** temporary set to false ************
	systemData.noGui = false;

	systemData.lastSettingsFile = systemData.dataDirectory + "settings" + QDir::separator() + QString("settings.fract");
	systemData.lastImageFile = systemData.dataDirectory + "images" + QDir::separator() + QString("image.jpg");
	systemData.lastImagePaletteFile = systemData.sharedDir + "textures" + QDir::separator() + QString("colour palette.jpg");

	QLocale systemLocale = QLocale::system();
	systemData.decimalPoint = systemLocale.decimalPoint();
	WriteLogString("Decimal point", QString(systemData.decimalPoint));

	systemData.supportedLanguages.insert("en_EN", "English");
	systemData.supportedLanguages.insert("pl_PL", "Polski");
	systemData.supportedLanguages.insert("de_DE", "Deutsch");
	systemData.supportedLanguages.insert("it_IT", "Italiano");

	return true;
}

int get_cpu_count()
{
	return QThread::idealThreadCount ();
}

void WriteLog(QString text)
{
	FILE *logfile = fopen(systemData.logfileName.toUtf8().constData(), "a");
	QString logtext = QString("PID: %1, time: %2, %3\n")
			.arg(QCoreApplication::applicationPid())
			.arg(QString::number(clock() / 1.0e6, 'f', 6))
			.arg(text);

	fputs(logtext.toUtf8().constData(), logfile);
	fclose(logfile);

	// write to log in window
	if(gMainInterface && gMainInterface->mainWindow != NULL)
	{
		gMainInterface->mainWindow->AppendToLog(logtext);
	}
}

void WriteLogString(QString text, QString value)
{
	WriteLog(text + ", value = " + value);
}

void WriteLogDouble(QString text, double value)
{
	WriteLog(text + ", value = " + QString::number(value));
}

bool CreateDefaultFolders(void)
{
	//create data directory if not exists
		bool result = true;

		result &= CreateDirectory(systemData.dataDirectory);
		result &= CreateDirectory(systemData.dataDirectory + "images");
		result &= CreateDirectory(systemData.dataDirectory + "keyframes");
		result &= CreateDirectory(systemData.dataDirectory + "paths");
		result &= CreateDirectory(systemData.dataDirectory + "undo");
		result &= CreateDirectory(systemData.dataDirectory + "paths");
		result &= CreateDirectory(systemData.dataDirectory + "thumbnails");
		result &= CreateDirectory(systemData.dataDirectory + "toolbar");

		// if(!QFileInfo(systemData.dataDirectory + "toolbar/default.fract").exists())
		if(QDir(systemData.dataDirectory + "toolbar").entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0)
		{
			// first run of program (or all toolbar items deleted) -> copy toolbar presets to working folder
			QDirIterator toolbarFiles(systemData.sharedDir + "toolbar");
			while (toolbarFiles.hasNext())
			{
				toolbarFiles.next();
				if(toolbarFiles.fileName() == "." || toolbarFiles.fileName() == "..") continue;
				fcopy(toolbarFiles.filePath(), systemData.dataDirectory + "toolbar/" + toolbarFiles.fileName());
			}
		}

#ifdef CLSUPPORT
		string oclDir = systemData.dataDirectory + "/custom_ocl_formulas";
		QString qoclDir = QString::fromStdString(oclDir);
		if(!QDir(qoclDir).exists())
		{
			result &= CreateDirectory(oclDir);
			if(result)
			{
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1.c", oclDir + "/cl_example1.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2.c", oclDir + "/cl_example2.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3.c", oclDir + +"/cl_example3.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1Init.c", oclDir + "/cl_example1Init.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2Init.c", oclDir + "/cl_example2Init.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3Init.c", oclDir + "/cl_example3Init.c");
			}
		}
#endif

		actualFileNames.actualFilenameSettings = QString("settings") + QDir::separator() + "default.fract";
		actualFileNames.actualFilenameImage = QString("images") + QDir::separator() + "image.jpg";
		actualFileNames.actualFilenamePalette = systemData.sharedDir + "textures" + QDir::separator() + "colour palette.jpg";

		return result;
}

bool CreateDirectory(QString qname)
{
	if(QDir(qname).exists())
	{
		WriteLogString("Directory already exists", qname);
		return true;
	}
	else
	{
		if(QDir().mkdir(qname))
		{
			WriteLogString("Directory created", qname);
			return true;
		}
		else
		{
			WriteLogString("Directory can't be created", qname);
			qCritical() << "error: directory " << qname << " cannot be created" << endl;
			return false;
		}
	}
}

void DeleteAllFilesFromDirectory(QString folder, QString filterExpression)
{
	QRegExp rx(filterExpression);
	rx.setPatternSyntax(QRegExp::Wildcard);

	if(QDir(folder).exists())
	{
		QDirIterator folderIterator(folder);
		while (folderIterator.hasNext())
		{
			folderIterator.next();
			if(folderIterator.fileName() == "." || folderIterator.fileName() == "..") continue;
			if(rx.exactMatch(folderIterator.fileName()))
			{
				if(QFile::remove(folderIterator.filePath()))
				{
					WriteLogString("File deleted", folderIterator.filePath());
				}
				else
				{
					WriteLogString("File not deleted", folderIterator.filePath());
				}
			}
		}
	}
	else
	{
		WriteLogString("Directory does not exist", folder);
	}
}

int fcopy(QString source, QString dest)
{
	// ------ file reading

	FILE * pFile;
	long int lSize;
	char *buffer;
	size_t result;

	pFile = fopen(source.toUtf8().constData(), "rb");
	if (pFile == NULL)
	{
		qCritical() << "Can't open source file for copying: " << source << endl;
		WriteLogString("Can't open source file for copying", source);
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	if(lSize > 0)
	{
		buffer = new char[lSize];

		// copy the file into the buffer:
		result = fread(buffer, 1, lSize, pFile);
		if (result != (size_t)lSize)
		{
			qCritical() << "Can't read source file for copying: " << source << endl;
			WriteLogString("Can't read source file for copying", source);
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

	pFile = fopen(dest.toUtf8().constData(), "wb");
	if (pFile == NULL)
	{
		qCritical() << "Can't open destination file for copying: " << dest << endl;
		WriteLogString("Can't open destination file for copying", dest);
		delete[] buffer;
		return 3;
	}
	fwrite(buffer, 1, lSize, pFile);
	fclose(pFile);

	delete [] buffer;
	WriteLogString("File copied", dest);
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
    switch (type) {
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
        fprintf(stderr, "Warning: %s\n(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        text = QString("Warning: ") + QString(localMsg.constData()) + " (" + context.file + ":" + QString::number(context.line) + ", " + context.function;
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        text = QString("Critical: ") + QString(localMsg.constData()) + " (" + context.file + ":" + QString::number(context.line) + ", " + context.function;
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        text = QString("Fatal: ") + QString(localMsg.constData()) + " (" + context.file + ":" + QString::number(context.line) + ", " + context.function;
        abort();
    }

    WriteLog(text);
}

void UpdateDefaultPaths (void)
{
	systemData.lastSettingsFile = gPar->Get<QString>("default_settings_path") + QDir::separator() + QString("settings.fract");
  systemData.lastImageFile = gPar->Get<QString>("default_image_path") + QDir::separator() + QString("image.jpg");
	gPar->Set("file_background", gPar->Get<QString>("default_textures_path") + QDir::separator() + "background.jpg");
	gPar->Set("file_envmap", gPar->Get<QString>("default_textures_path") + QDir::separator() + "envmap.jpg");
	gPar->Set("file_lightmap", gPar->Get<QString>("default_textures_path") + QDir::separator() + "lightmap.jpg");
}

void UpdateUIStyle (void)
{
	// set ui style
	if(gPar->Get<int>("ui_style_type") >= 0 && gPar->Get<int>("ui_style_type") < QStyleFactory::keys().size())
	{
		gApplication->setStyle(QStyleFactory::create(QStyleFactory::keys().at(gPar->Get<int>("ui_style_type"))));
	}
}

void UpdateUISkin (void)
{
	static QPalette defaultPalette; // cache "normal" skin on first call
	QPalette palette;

	switch(gPar->Get<int>("ui_skin"))
	{
		case 1: // dark skin
			palette.setColor(QPalette::Window, QColor(53,53,53));
			palette.setColor(QPalette::WindowText, Qt::white);
			palette.setColor(QPalette::Base, QColor(25,25,25));
			palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
			palette.setColor(QPalette::ToolTipBase, Qt::white);
			palette.setColor(QPalette::ToolTipText, Qt::white);
			palette.setColor(QPalette::Text, Qt::white);
			palette.setColor(QPalette::Button, QColor(53,53,53));
			palette.setColor(QPalette::ButtonText, Qt::white);
			palette.setColor(QPalette::BrightText, Qt::red);
			palette.setColor(QPalette::Link, QColor(42, 130, 218));

			palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
			palette.setColor(QPalette::HighlightedText, Qt::black);
			palette.setColor(QPalette::ToolTipBase, Qt::black);
			palette.setColor(QPalette::ToolTipText, QColor(42, 130, 218));
		break;
		case 2: // light skin (only roughly inverted dark skin)
			palette.setColor(QPalette::Window, QColor(240,240,240));
			palette.setColor(QPalette::WindowText, Qt::black);
			palette.setColor(QPalette::Base, QColor(250,250,250));
			palette.setColor(QPalette::AlternateBase, QColor(240,240,240));
			palette.setColor(QPalette::ToolTipBase, Qt::black);
			palette.setColor(QPalette::ToolTipText, Qt::black);
			palette.setColor(QPalette::Text, Qt::black);
			palette.setColor(QPalette::Button, QColor(240,240,240));
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

void UpdateLanguage (void)
{
	// Set language from locale
	WriteLog("Prepare translator");
	static QTranslator main_translator;
	static QTranslator qt_data_translator;

	QString locale = systemData.locale.name();
	if(systemData.supportedLanguages.contains(gPar->Get<QString>("language")))
	{
		locale = gPar->Get<QString>("language");
	}

	WriteLogString("locale", locale);
	main_translator.load(locale, systemData.sharedDir + QDir::separator() + "language");
	qt_data_translator.load("qt_data_" + locale, systemData.sharedDir + QDir::separator() + "language");

	WriteLog("Instaling translator");
	gApplication->installTranslator(&main_translator);
	gApplication->installTranslator(&qt_data_translator);
}



