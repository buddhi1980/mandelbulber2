/*
 * error_message.cpp
 *
 *  Created on: Jul 10, 2014
 *      Author: krzysztof
 */

#include "error_message.hpp"
#include <QMessageBox>

void cErrorMessage::showMessage(QString text, enumMessageType messageType, QWidget *parent)
{
	QTextStream out(stdout);
	QTextStream outErr(stderr);

	QString messageText;
	if (messageType == warningMessage) messageText = "Warning: ";
	else if (messageType == errorMessage) messageText = "Error: ";
	else if (messageType == infoMessage) messageText = "Note:";

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
			messageBox->setWindowTitle(QString("Mandelbulber warning"));
			messageBox->setIcon(QMessageBox::Warning);
		}
		else if (messageType == errorMessage)
		{
			messageBox->setWindowTitle(QString("Mandelbulber error"));
			messageBox->setIcon(QMessageBox::Critical);
		}
		else if (messageType == infoMessage)
		{
			messageBox->setWindowTitle(QString("Mandelbulber information"));
			messageBox->setIcon(QMessageBox::Information);
		}
		messageBox->exec();
		delete messageBox;
	}
}


