/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cErrorMessage class - displaying warning/error messages as popup window and stderr output
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

#include "error_message.hpp"
#include <QMessageBox>

void cErrorMessage::showMessage(QString text, enumMessageType messageType, QWidget *parent)
{
	QTextStream out(stdout);
	QTextStream outErr(stderr);

	QString messageText;
	if (messageType == warningMessage) messageText = QObject::tr("Warning");
	else if (messageType == errorMessage) messageText = QObject::tr("Error");
	else if (messageType == infoMessage) messageText = QObject::tr("Note");

	messageText += ": ";
	messageText += text;

	WriteLog(messageText);

	if (messageType == warningMessage) outErr << messageText + "\n"  << flush;
	else if (messageType == errorMessage) outErr << messageText + "\n" << flush;
	else if (messageType == infoMessage) out << messageText + "\n";

	if(!systemData.noGui)
	{
		QMessageBox *messageBox = new QMessageBox(parent);
		messageBox->setText(messageText);

		if (messageType == warningMessage)
		{
			messageBox->setWindowTitle(QObject::tr("Mandelbulber warning"));
			messageBox->setIcon(QMessageBox::Warning);
		}
		else if (messageType == errorMessage)
		{
			messageBox->setWindowTitle(QObject::tr("Mandelbulber error"));
			messageBox->setIcon(QMessageBox::Critical);
		}
		else if (messageType == infoMessage)
		{
			messageBox->setWindowTitle(QObject::tr("Mandelbulber information"));
			messageBox->setIcon(QMessageBox::Information);
		}
		messageBox->exec();
		delete messageBox;
	}
}


