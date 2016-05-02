/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyLineEdit class - promoted MyLineEdit widget with context menu
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


#include "mylineedit.h"
#include "../src/parameters.hpp"
#include "../src/global_data.hpp"

MyLineEdit::MyLineEdit(QWidget *parent) :
		QLineEdit(parent)
{
	actionResetToDefault = NULL;
	actionResetVectorToDefault = NULL;
	actionAddToFlightAnimation = NULL;
	actionAddToKeyframeAnimation = NULL;
	actionCopyVectorToClipboard = NULL;
	actionPasteVectorFromClipboard = NULL;
	parameterContainer = NULL;
	gotDefault = false;
}

void MyLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = createStandardContextMenu();
	menu->addSeparator();

	QString type = GetType(objectName());
	if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
	{
		actionCopyVectorToClipboard = menu->addAction(tr("Copy vector"));
		actionPasteVectorFromClipboard = menu->addAction(tr("Paste vector"));
		actionResetVectorToDefault = menu->addAction(tr("Reset vector to default"));
	}
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	actionAddToFlightAnimation = menu->addAction(tr("Add to flight animation"));
	actionAddToKeyframeAnimation = menu->addAction(tr("Add to keyframe animation"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				setText(defaultText);
				emit editingFinished();
			}
			else
			{
				qCritical() << "MyLineEdit::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
		else if (selectedItem == actionAddToFlightAnimation)
		{
			if (parameterContainer)
			{
				QString parName = parameterName;
				if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
					parName = parameterName.left(parameterName.length() - 2);

				gAnimFrames->AddAnimatedParameter(parName, parameterContainer->GetAsOneParameter(parName));
				gFlightAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionAddToKeyframeAnimation)
		{
			if (parameterContainer)
			{
				QString parName = parameterName;
				if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
					parName = parameterName.left(parameterName.length() - 2);

				gKeyframes->AddAnimatedParameter(parName, parameterContainer->GetAsOneParameter(parName));
				gKeyframeAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionCopyVectorToClipboard)
		{
			CopyToClipboard();
		}
		else if (selectedItem == actionPasteVectorFromClipboard)
		{
			PasteFromClipboard();
		}
		else if (selectedItem == actionResetVectorToDefault)
		{
			ResetVectorToDefault();
		}
	}
	delete menu;
}

QString MyLineEdit::GetType(const QString &name)
{
	size_t firstDashPosition = name.indexOf("_");
	return name.left(firstDashPosition);
}

void MyLineEdit::paintEvent(QPaintEvent *event)
{
	if (text() != GetDefault())
	{
		QFont f = font();
		f.setBold(true);
		setFont(f);
	}
	else
	{
		QFont f = font();
		f.setBold(false);
		setFont(f);
	}
	QLineEdit::paintEvent(event);
}

QString MyLineEdit::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("logvect3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector3 val = parameterContainer->GetDefault<CVector3>(nameVect);
			QString valS = QString("%L1").arg(val.itemByName(lastChar), 0, 'g', 16);
			defaultText = valS;
			gotDefault = true;
		}
		else if (type == QString("vect4") || type == QString("logvect3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector4 val = parameterContainer->GetDefault<CVector4>(nameVect);
			QString valS = QString("%L1").arg(val.itemByName(lastChar), 0, 'g', 16);
			defaultText = valS;
			gotDefault = true;
		}
		else
		{
			QString val = parameterContainer->GetDefault<QString>(parameterName);
			defaultText = val;
			gotDefault = true;
		}

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "<br>";
		toolTipText += "Default: " + defaultText;
		setToolTip(toolTipText);
	}
	return defaultText;
}

void MyLineEdit::CopyToClipboard()
{
	if (parameterContainer)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
		{
			QString nameVect = parameterName.left(parameterName.length() - 2);
			QString valS = parameterContainer->Get<QString>(nameVect);
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(valS);
		}
	}
}

void MyLineEdit::PasteFromClipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = clipboard->text();

	if (parameterContainer)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("logvect3"))
		{
			QStringList numbersStr = text.split(' ');
			if (numbersStr.length() == 3)
			{
				QString nameVect = parameterName.left(parameterName.length() - 2);
				QString lineEditNameX, lineEditNameY, lineEditNameZ;
				if(type == QString("vect3"))
				{
					lineEditNameX = QString("vect3_%1_x").arg(nameVect);
					lineEditNameY = QString("vect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("vect3_%1_z").arg(nameVect);
				}
				else if(type == QString("logvect3"))
				{
					lineEditNameX = QString("logvect3_%1_x").arg(nameVect);
					lineEditNameY = QString("logvect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("logvect3_%1_z").arg(nameVect);
				}
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit*>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit*>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit*>(lineEditNameZ);
				if (lineEditX) lineEditX->setText(numbersStr[0]);
				if (lineEditY) lineEditY->setText(numbersStr[1]);
				if (lineEditZ) lineEditZ->setText(numbersStr[2]);
			}
			emit editingFinished();
		}

		if (type == QString("vect4"))
		{
			QStringList numbersStr = text.split(' ');
			if (numbersStr.length() == 4)
			{
				QString nameVect = parameterName.left(parameterName.length() - 2);
				QString lineEditNameX = QString("vect4_%1_x").arg(nameVect);
				QString lineEditNameY = QString("vect4_%1_y").arg(nameVect);
				QString lineEditNameZ = QString("vect4_%1_z").arg(nameVect);
				QString lineEditNameW = QString("vect4_%1_w").arg(nameVect);
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit*>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit*>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit*>(lineEditNameZ);
				MyLineEdit *lineEditW = parent()->findChild<MyLineEdit*>(lineEditNameW);
				if (lineEditX) lineEditX->setText(numbersStr[0]);
				if (lineEditY) lineEditY->setText(numbersStr[1]);
				if (lineEditZ) lineEditZ->setText(numbersStr[2]);
				if (lineEditW) lineEditW->setText(numbersStr[3]);
			}
			emit editingFinished();
		}
	}
}

void MyLineEdit::ResetVectorToDefault()
{
	if (parameterContainer)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
		{
			QString nameVect = parameterName.left(parameterName.length() - 2);
			QStringList numbersStr = parameterContainer->GetDefault<QString>(nameVect).split(' ');
			if(numbersStr.length() >= 3)
			{
				QString lineEditNameX, lineEditNameY, lineEditNameZ;
				if(type == QString("vect3"))
				{
					lineEditNameX = QString("vect3_%1_x").arg(nameVect);
					lineEditNameY = QString("vect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("vect3_%1_z").arg(nameVect);
				}
				else if(type == QString("logvect3"))
				{
					lineEditNameX = QString("logvect3_%1_x").arg(nameVect);
					lineEditNameY = QString("logvect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("logvect3_%1_z").arg(nameVect);
				}
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit*>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit*>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit*>(lineEditNameZ);
				if (lineEditX) lineEditX->setText(numbersStr[0]);
				if (lineEditY) lineEditY->setText(numbersStr[1]);
				if (lineEditZ) lineEditZ->setText(numbersStr[2]);
			}
			if (type == QString("vect4") && numbersStr.length() == 4)
			{
				QString lineEditNameW = QString("vect3_%1_w").arg(nameVect);
				MyLineEdit *lineEditW = parent()->findChild<MyLineEdit*>(lineEditNameW);
				if (lineEditW) lineEditW->setText(numbersStr[3]);
			}
			emit editingFinished();
		}
	}
}
