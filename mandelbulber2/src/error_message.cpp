/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cErrorMessage class - displaying warning/error messages as popup window and stderr output
 */

#include "error_message.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QTextStream>

#include "global_data.hpp"
#include "headless.h"
#include "write_log.hpp"

cErrorMessage *gErrorMessage = nullptr;

QString cErrorMessage::lastMessage;
QElapsedTimer cErrorMessage::lastMessageTime;

cErrorMessage::cErrorMessage(QObject *parent) : QObject(parent)
{
	connect(this, &cErrorMessage::signalShowMessage, this, &cErrorMessage::slotShowMessage,
		Qt::QueuedConnection);
	lastMessageTime.start();
}

void cErrorMessage::showMessage(QString text, enumMessageType messageType, QWidget *parent)
{
	QTextStream out(stdout);
	QTextStream outErr(stderr);

	QString messageText;

	if (qobject_cast<QApplication *>(gApplication))
	{
		if (messageType == warningMessage)
			messageText = QObject::tr("Warning");
		else if (messageType == errorMessage)
			messageText = QObject::tr("Error");
		else if (messageType == infoMessage)
			messageText = QObject::tr("Note");

		messageText += ": ";
		messageText += text;

		WriteLog(messageText, 1);

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
		if (messageType == warningMessage)
			outErr << messageText << "\n" << flush;
		else if (messageType == errorMessage)
			outErr << messageText << "\n" << flush;
#else
		if (messageType == warningMessage)
			outErr << messageText << "\n" << Qt::flush;
		else if (messageType == errorMessage)
			outErr << messageText << "\n" << Qt::flush;
#endif
		else if (messageType == infoMessage)
			out << messageText << "\n";

		if ((lastMessage != text && lastMessageTime.elapsed() > 1000) || lastMessage.isEmpty())
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
			int result = messageBox->exec();
			Q_UNUSED(result);
			delete messageBox;
			lastMessageTime.restart();
		}
	}
	else
	{
		QString header;
		if (messageType == warningMessage)
		{
			header = cHeadless::colorize(
				QObject::tr("\nWarning: "), cHeadless::ansiMagenta, cHeadless::noExplicitColor, true);
			messageText =
				cHeadless::colorize(text, cHeadless::ansiMagenta, cHeadless::noExplicitColor, false);
		}
		else if (messageType == errorMessage)
		{
			header = cHeadless::colorize(
				QObject::tr("\nError: "), cHeadless::ansiRed, cHeadless::ansiWhite, true);
			messageText = cHeadless::colorize(text, cHeadless::ansiRed, cHeadless::ansiWhite, false);
		}
		else if (messageType == infoMessage)
		{
			header = cHeadless::colorize(
				QObject::tr("\nNote: "), cHeadless::ansiGreen, cHeadless::noExplicitColor, true);
			messageText =
				cHeadless::colorize(text, cHeadless::ansiGreen, cHeadless::noExplicitColor, false);
		}

		out << header << messageText << "\n";
	}
	lastMessage = text;
}

void cErrorMessage::slotShowMessage(QString text, enumMessageType messageType, QWidget *parent)
{
	showMessage(text, messageType, parent);
}

void cErrorMessage::showMessageFromOtherThread(
	QString text, enumMessageType messageType, QWidget *parent)
{
	emit signalShowMessage(text, messageType, parent);
}
