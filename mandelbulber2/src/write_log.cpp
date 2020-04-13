/*
 * write_log.cpp
 *
 *  Created on: 13 kwi 2020
 *      Author: krzysztof
 */

#include "write_log.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
//#include <ctime>
#include <iostream>
#include <QCoreApplication>

#include "interface.hpp"
#include "render_window.hpp"
#include "system_data.hpp"

using namespace std;

void WriteLogCout(const QString &text, int verbosityLevel)
{
	// output to console
	cout << text.toStdString();
	cout.flush();
	WriteLog(text, verbosityLevel);
}

void WriteLog(const QString &text, int verbosityLevel)
{
	// verbosity level:
	// 1 - only errors
	// 2 - main events / actions
	// 3 - detailed events / actions

	QMutex mutex;

	if (verbosityLevel <= systemData.loggingVerbosity)
	{
		mutex.lock();
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
		if (logfile)
		{
			fputs(logText.toLocal8Bit().constData(), logfile);
			fclose(logfile);
		}
		mutex.unlock();
		// write to log in window
		if (gMainInterface && gMainInterface->mainWindow != nullptr)
		{
			emit gMainInterface->mainWindow->AppendToLog(logText);
		}
	}
}

void WriteLogString(const QString &text, const QString &value, int verbosityLevel)
{
	WriteLog(text + ", value = " + value, verbosityLevel);
}

void WriteLogDouble(const QString &text, double value, int verbosityLevel)
{
	WriteLog(text + ", value = " + QString::number(value), verbosityLevel);
}

void WriteLogInt(const QString &text, int value, int verbosityLevel)
{
	WriteLog(text + ", value = " + QString::number(value), verbosityLevel);
}

void WriteLogSizeT(const QString &text, size_t value, int verbosityLevel)
{
	WriteLog(text + ", value = " + QString::number(value), verbosityLevel);
}
