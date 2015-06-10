/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * QPlainTextEdit class - promoted QPlainTextEdit widget for Log File view
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
 * Authors: Sebastian Jennen
 */

#include "mylogwidget.h"

void MyLogWidget::appendMessage(const QString& text)
{
	if(!initializedFromLogFile)
	{
		this->initFromLogFile();
		initializedFromLogFile = true;
	}
	else
	{
		moveCursor (QTextCursor::End);
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

QString MyLogWidget::formatLine(const QString& text)
{
	QRegularExpression re("^(PID:) ([0-9]+), (time:) ([0-9.]+), (.*)");
	QRegularExpressionMatch match = re.match(text);
	if (match.hasMatch())
	{
		QString out = "<span style=\"color: grey;\">" + match.captured(1) + " <b>" + match.captured(2) + "</b></span>, "
			+ "<span style=\"color: orange;\">" + match.captured(3) + " <b>" + match.captured(4) + "</b></span>, ";
		QRegularExpression reType("^(Debug|Warning|Critical|Error|NetRender|Gamepad)(.*)");
		QRegularExpressionMatch matchType = reType.match(match.captured(5));
		if (matchType.hasMatch())
		{
			QString color = "black";
			if(matchType.captured(1) == "Debug") color = "green";
			else if(matchType.captured(1) == "Warning") color = "orange";
			else if(matchType.captured(1) == "Critical") color = "red";
			else if(matchType.captured(1) == "Error") color = "red";
			else if(matchType.captured(1) == "NetRender") color = "darkblue";
			else if(matchType.captured(1) == "Gamepad") color = "purple";

			out += "<span style=\"color: " + color + ";\">" + matchType.captured(1) + "<b>" + matchType.captured(2) + "</b></span>";
		}
		else
		{
			out += "<span style=\"color: black;\"><b>" + match.captured(5) + "</b></span>";
		}
		return out;
	}
	else
	{
		return text;
	}
}
