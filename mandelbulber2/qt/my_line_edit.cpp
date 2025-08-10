/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * MyLineEdit class - promoted MyLineEdit widget with context menu
 */

#include "my_line_edit.h"

#include <QApplication>
#include <QByteArray>
#include <QClipboard>
#include <QDebug>
#include <QLocale>
#include <QMenu>
#include <QSlider>
#include <QWheelEvent>

#include "frame_slider_popup.h"

#include "src/animation_flight.hpp"
#include "src/animation_keyframes.hpp"
#include "src/common_math.h"
#include "src/system_data.hpp"

MyLineEdit::MyLineEdit(QWidget *parent) : QLineEdit(parent), CommonMyWidgetWrapper(this)
{
	actionResetVectorToDefault = nullptr;
	actionCopyVectorToClipboard = nullptr;
	actionPasteVectorFromClipboard = nullptr;
	slider = nullptr;
	valueBeforeSliderDrag = 0.0;
	precision = enumSliderPrecision::precisionNormal;
}

MyLineEdit::~MyLineEdit()
{
	if (slider) delete slider;
}

void MyLineEdit::paintEvent(QPaintEvent *event)
{
	QFont f = font();
	f.setBold(text() != GetDefault());
	setFont(f);
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
			QString valS = QString("%L1").arg(val.itemByName(lastChar), 0, 'g', 15);
			defaultText = valS;
			gotDefault = true;
		}
		else if (type == QString("vect4") || type == QString("logvect3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector4 val = parameterContainer->GetDefault<CVector4>(nameVect);
			QString valS = QString("%L1").arg(val.itemByName(lastChar), 0, 'g', 15);
			defaultText = valS;
			gotDefault = true;
		}
		else
		{
			QString val = parameterContainer->GetDefault<QString>(parameterName);
			defaultText = val;
			gotDefault = true;
		}

		setToolTipText();
	}
	return defaultText;
}

void MyLineEdit::CopyToClipboard() const
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
				QString lineEditNameX;
				QString lineEditNameY;
				QString lineEditNameZ;
				if (type == QString("vect3"))
				{
					lineEditNameX = QString("vect3_%1_x").arg(nameVect);
					lineEditNameY = QString("vect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("vect3_%1_z").arg(nameVect);
				}
				else if (type == QString("logvect3"))
				{
					lineEditNameX = QString("logvect3_%1_x").arg(nameVect);
					lineEditNameY = QString("logvect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("logvect3_%1_z").arg(nameVect);
				}
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit *>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit *>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit *>(lineEditNameZ);
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
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit *>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit *>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit *>(lineEditNameZ);
				MyLineEdit *lineEditW = parent()->findChild<MyLineEdit *>(lineEditNameW);
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
			if (numbersStr.length() >= 3)
			{
				QString lineEditNameX;
				QString lineEditNameY;
				QString lineEditNameZ;

				if (type == QString("vect3"))
				{
					lineEditNameX = QString("vect3_%1_x").arg(nameVect);
					lineEditNameY = QString("vect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("vect3_%1_z").arg(nameVect);
				}
				else if (type == QString("logvect3"))
				{
					lineEditNameX = QString("logvect3_%1_x").arg(nameVect);
					lineEditNameY = QString("logvect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("logvect3_%1_z").arg(nameVect);
				}
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit *>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit *>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit *>(lineEditNameZ);
				if (lineEditX) lineEditX->setText(numbersStr[0]);
				if (lineEditY) lineEditY->setText(numbersStr[1]);
				if (lineEditZ) lineEditZ->setText(numbersStr[2]);
			}
			if (type == QString("vect4") && numbersStr.length() == 4)
			{
				QString lineEditNameW = QString("vect3_%1_w").arg(nameVect);
				MyLineEdit *lineEditW = parent()->findChild<MyLineEdit *>(lineEditNameW);
				if (lineEditW) lineEditW->setText(numbersStr[3]);
			}
			emit editingFinished();
		}
	}
}

void MyLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = createStandardContextMenu();

	QString type = GetType(objectName());
	if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
	{
		menu->addSeparator();
		actionCopyVectorToClipboard = menu->addAction(tr("Copy vector"));
		actionPasteVectorFromClipboard = menu->addAction(tr("Paste vector"));
		actionResetVectorToDefault = menu->addAction(tr("Reset vector to default"));

		QIcon iconReset = QIcon(":system/icons/edit-undo.png");
		actionResetVectorToDefault->setIcon(iconReset);
		QIcon iconCopy = QIcon::fromTheme("edit-copy");
		actionCopyVectorToClipboard->setIcon(iconCopy);
		QIcon iconPaste = QIcon::fromTheme("edit-paste");
		actionPasteVectorFromClipboard->setIcon(iconPaste);

		connect(actionCopyVectorToClipboard, SIGNAL(triggered()), this, SLOT(slotCopyVector()));
		connect(actionPasteVectorFromClipboard, SIGNAL(triggered()), this, SLOT(slotPasteVector()));
		connect(actionResetVectorToDefault, SIGNAL(triggered()), this, SLOT(slotResetVector()));
	}
	CommonMyWidgetWrapper::contextMenuEvent(event, menu);
}

void MyLineEdit::resetToDefault()
{
	setText(defaultText);
	emit editingFinished();
}

QString MyLineEdit::getDefaultAsString()
{
	return defaultText;
}

QString MyLineEdit::getFullParameterName()
{
	QString type = GetType(objectName());
	QString parName = parameterName;
	if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
		parName = parameterName.left(parameterName.length() - 2);
	return parName;
}

void MyLineEdit::slotCopyVector() const
{
	CopyToClipboard();
}

void MyLineEdit::slotPasteVector()
{
	PasteFromClipboard();
}

void MyLineEdit::slotResetVector()
{
	ResetVectorToDefault();
}

void MyLineEdit::focusInEvent(QFocusEvent *event)
{
	QLineEdit::focusInEvent(event);

	QString type = GetType(objectName());
	if (type != "text")
	{
		if (!slider)
		{
			QWidget *topWidget = window();
			slider = new cFrameSliderPopup(topWidget);
			slider->setFocusPolicy(Qt::NoFocus);
			slider->hide();
		}

		QWidget *topWidget = window();
		QPoint windowPoint = mapTo(topWidget, QPoint());
		int width = this->width();
		int hOffset = height();

		QSize minimumSize = slider->minimumSizeHint();
		width = std::max(width, int(minimumSize.width() * 0.6));

		slider->adjustSize();
		slider->setFixedWidth(width);

		if (windowPoint.y() + slider->height() + hOffset > topWidget->height())
			hOffset = -slider->height();

		slider->move(windowPoint.x(), windowPoint.y() + hOffset);
		slider->show();

		precision = enumSliderPrecision::precisionNormal;
		connect(slider, &cFrameSliderPopup::changedPrecision, this, &MyLineEdit::slotChangedPrecision);

		// connect(slider, SIGNAL(timerTrigger()), this, SLOT(slotSliderTimerUpdateValue()));
		connect(slider, SIGNAL(resetPressed()), this, SLOT(slotResetToDefault()));
		connect(slider, SIGNAL(zeroPressed()), this, SLOT(slotZeroValue()));
		connect(slider, SIGNAL(minusPressed()), this, SLOT(slotInvertSign()));
		connect(slider, SIGNAL(halfPressed()), this, SLOT(slotHalfValue()));
		connect(slider, SIGNAL(doublePressed()), this, SLOT(slotDoubleValue()));
		connect(slider, SIGNAL(intPressed()), this, SLOT(slotRoundValue()));
		connect(slider, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()));
		connect(slider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
		connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(slotSliderMoved(int)));
	}
}

void MyLineEdit::focusOutEvent(QFocusEvent *event)
{
	QLineEdit::focusOutEvent(event);

	if (slider)
	{
		slider->disconnect();
		slider->hide();
		slider->deleteLater();
		slider = nullptr;
	}
}

void MyLineEdit::slotSliderTimerUpdateValue()
{
	double value = systemData.locale.toDouble(text());
	int iDiff = slider->value() - 500;

	if (iDiff != 0)
	{
		double sign = (iDiff > 0) ? 1.0 : -1.0;

		if (objectName().left(3) == QString("log"))
		{
			double dDiff = iDiff / 1000.0;
			double change = pow(10.0, dDiff * dDiff * sign);
			const QString text = QString("%L1").arg(value * change, 0, 'g', 15);
			setText(text);
		}
		else
		{
			double dDiff = abs(iDiff) / 33.0 - 15.0;
			double change = pow(10.0, dDiff) * sign;
			const QString text = QString("%L1").arg(value + change, 0, 'g', 15);
			setText(text);
		}
		emit editingFinished();
	}
}

void MyLineEdit::slotResetToDefault()
{
	resetToDefault();
	emit returnPressed();
}

void MyLineEdit::slotZeroValue()
{
	setText("0");
	emit returnPressed();
}

void MyLineEdit::slotDoubleValue()
{
	const double value = systemData.locale.toDouble(text());
	const QString text = QString("%L1").arg(value * 2.0, 0, 'g', 15);
	setText(text);
	emit returnPressed();
}

void MyLineEdit::slotHalfValue()
{
	const double value = systemData.locale.toDouble(text());
	const QString text = QString("%L1").arg(value * 0.5, 0, 'g', 15);
	setText(text);
	emit returnPressed();
}

void MyLineEdit::slotRoundValue()
{
	const double value = systemData.locale.toDouble(text());
	const QString text = QString("%L1").arg(MagicRound(value, 0.05), 0, 'g', 15);
	setText(text);
	emit returnPressed();
}

void MyLineEdit::slotInvertSign()
{
	const double value = systemData.locale.toDouble(text());
	const QString text = QString("%L1").arg(value * (-1.0), 0, 'g', 15);
	setText(text);
	emit returnPressed();
}

void MyLineEdit::wheelEvent(QWheelEvent *event)
{
	if (slider) // if it's edit field with slider (not text)
	{
		double dPrecision = 1.0;
		switch (precision)
		{
			case enumSliderPrecision::precisionSuperFine: dPrecision = 0.0001; break;
			case enumSliderPrecision::precisionVeryFine: dPrecision = 0.001; break;
			case enumSliderPrecision::precisionFine: dPrecision = 0.01; break;
			case enumSliderPrecision::precisionNormal: dPrecision = 0.1; break;
			case enumSliderPrecision::precisionCoarse: dPrecision = 0.5; break;
		}

		double value = systemData.locale.toDouble(text());

		double change = event->angleDelta().y() / 360.0;

		if (qIsNull(value)) value = (change > 0.0) ? 0.1 : -0.1;

		if (value < 0.0) change *= -1.0;

		double multiplier = (1.0 + change * dPrecision);

		const QString text = QString("%L1").arg(value * multiplier, 0, 'g', 15);
		setText(text);
		emit returnPressed();
		event->accept();
	}
}

void MyLineEdit::slotSliderPressed()
{
	valueBeforeSliderDrag = systemData.locale.toDouble(text());
}

void MyLineEdit::slotSliderReleased()
{
	emit returnPressed();
}

void MyLineEdit::slotSliderMoved(int sliderPosition)
{
	double newValue = valueBeforeSliderDrag;

	int iDiff = sliderPosition - 500;

	enumSliderPrecision precision = slider->precision();
	double dPrecision = 1.0;
	switch (precision)
	{
		case enumSliderPrecision::precisionSuperFine: dPrecision = 3.0; break;
		case enumSliderPrecision::precisionVeryFine: dPrecision = 2.0; break;
		case enumSliderPrecision::precisionFine: dPrecision = 1.0; break;
		case enumSliderPrecision::precisionNormal: dPrecision = 0.0; break;
		case enumSliderPrecision::precisionCoarse: dPrecision = -1.0; break;
	}

	double dDiff = iDiff / 500.0;
	double sign = (iDiff > 0) ? 1.0 : -1.0;

	if (valueBeforeSliderDrag == 0.0)
	{
		if (objectName().left(3) == QString("log"))
		{
			newValue = pow(10.0, dDiff - dPrecision - 1.0);
		}
		else
		{
			newValue = sign * pow(10.0, dDiff * dDiff * 3.0 - 3.0 - dPrecision);
		}
	}
	else
	{
		if (objectName().left(3) == QString("log"))
		{
			double change = pow(10.0, pow(dDiff * pow(1.0 / 2.5, dPrecision + 1), 3.0));
			newValue = valueBeforeSliderDrag * change;
		}
		else
		{
			double change = sign * pow(10.0, pow(fabs(dDiff), 0.3) * 10.0 - 10.0 - dPrecision)
											* fabs(valueBeforeSliderDrag);
			newValue = valueBeforeSliderDrag + change;
		}
	}

	const QString text = QString("%L1").arg(newValue, 0, 'g', 15);
	setText(text);
	emit editingFinished();
}

void MyLineEdit::slotChangedPrecision(enumSliderPrecision _precision)
{
	precision = _precision;
}

void MyLineEdit::UpdateScriptAppearance(bool hasScript)
{
	QFont f = font();
	f.setItalic(hasScript);
	setFont(f);

	if (hasScript)
		setStyleSheet("MyLineEdit {color: yellow;  background-color: blue}");
	else
		setStyleSheet(qobject_cast<QWidget *>(this->parent())->styleSheet());
}

// Add this method to MyLineEdit

void MyLineEdit::keyPressEvent(QKeyEvent *event)
{
	if (slider) // Only for numeric fields
	{
		double change = 1.0;
		switch (precision)
		{
			case enumSliderPrecision::precisionSuperFine: change = 0.00001; break; // 0.1%
			case enumSliderPrecision::precisionVeryFine: change = 0.0001; break;	 // 0.5%
			case enumSliderPrecision::precisionFine: change = 0.001; break;				 // 1%
			case enumSliderPrecision::precisionNormal: change = 0.01; break;			 // 10%
			case enumSliderPrecision::precisionCoarse: change = 0.1; break;				 // 50%
			default: change = 0.1; break;
		}

		double value = systemData.locale.toDouble(text());
		if (event->key() == Qt::Key_Up)
		{
			// if shift pressed
			if (event->modifiers() & Qt::ShiftModifier)
			{
				value += change;
			}
			else
			{
				value *= (1.0 + change);
			}
			setText(QString("%L1").arg(value, 0, 'g', 15));
			emit returnPressed();
			event->accept();
			return;
		}
		else if (event->key() == Qt::Key_Down)
		{
			if (event->modifiers() & Qt::ShiftModifier)
			{
				value -= change;
			}
			else
			{
				value *= (1.0 - change);
			}
			setText(QString("%L1").arg(value, 0, 'g', 15));
			emit returnPressed();
			event->accept();
			return;
		}
	}
	QLineEdit::keyPressEvent(event);
}
