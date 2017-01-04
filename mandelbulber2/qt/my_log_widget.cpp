/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * MyLogWidget - promoted QPlainTextEdit widget for log file display
 *
 * This widget monitors the logfile under systemData.logfileName.
 * The lines get parsed with regex and displayed auto-highlighted.
 */

#include "my_log_widget.h"
#include "../src/system.hpp"

MyLogWidget::MyLogWidget(QWidget *parent) : QPlainTextEdit(parent)
{
	setReadOnly(true);
	initializedFromLogFile = false;
	reBasic = new QRegularExpression("^(PID:) ([0-9]+), (time:) ([0-9.]+), (.*)");
	reInnerType = new QRegularExpression("^(Debug|Warning|Critical|Error|NetRender|Gamepad)(.*)");
}

MyLogWidget::~MyLogWidget()
{
	delete reBasic;
	delete reInnerType;
}

void MyLogWidget::appendMessage(const QString &text)
{
	if (!initializedFromLogFile)
	{
		this->initFromLogFile();
		initializedFromLogFile = true;
	}
	else
	{
		moveCursor(QTextCursor::End);
		this->appendHtml(formatLine(text));
	}
}

void MyLogWidget::initFromLogFile()
{
	QFile logFile(systemData.logfileName);
	logFile.open(QFile::ReadOnly | QFile::Text);
	QTextStream logTextStream(&logFile);
	while (!logTextStream.atEnd())
	{
		this->appendHtml(formatLine(logTextStream.readLine()));
	}
	this->ensureCursorVisible();
}

QString MyLogWidget::formatLine(const QString &text) const
{
	QRegularExpressionMatch match = reBasic->match(text);
	if (match.hasMatch())
	{
		QString out = "<span style=\"color: grey;\">" + match.captured(1) + " <b>" + match.captured(2)
									+ "</b></span>, " + "<span style=\"color: orange;\">" + match.captured(3) + " <b>"
									+ match.captured(4) + "</b></span>, ";
		QRegularExpressionMatch matchType = reInnerType->match(match.captured(5));
		if (matchType.hasMatch())
		{
			QString color = "";
			if (matchType.captured(1) == "Debug")
				color = "green";
			else if (matchType.captured(1) == "Warning")
				color = "orange";
			else if (matchType.captured(1) == "Critical")
				color = "red";
			else if (matchType.captured(1) == "Error")
				color = "red";
			else if (matchType.captured(1) == "NetRender")
				color = "darkblue";
			else if (matchType.captured(1) == "Gamepad")
				color = "purple";

			out += color != "" ? "<span style=\"color: " + color + ";\">" : "<span>";
			out += matchType.captured(1) + "<b>" + matchType.captured(2) + "</b></span>";
		}
		else
		{
			out += "<span><b>" + match.captured(5) + "</b></span>";
		}
		return out;
	}
	else
	{
		return text;
	}
}
