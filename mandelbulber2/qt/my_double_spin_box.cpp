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
 * MyDoubleSpinBox class - promoted QDoubleSpinBox widget with context menu
 */

#include "my_double_spin_box.h"

#include <QDebug>
#include <QLineEdit>

#include "frame_slider_popup.h"

#include "src/animation_flight.hpp"
#include "src/animation_keyframes.hpp"
#include "src/common_math.h"
#include "src/system.hpp"

MyDoubleSpinBox::MyDoubleSpinBox(QWidget *parent)
		: QDoubleSpinBox(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = 0;
	slider = nullptr;
	hasDial = false;
	valueBeforeSliderDrag = 0.0;
	precision = enumSliderPrecision::precisionNormal;
	defaultSingleStep = 1.0;
};

MyDoubleSpinBox::~MyDoubleSpinBox()
{
	if (slider) delete slider;
}

void MyDoubleSpinBox::paintEvent(QPaintEvent *event)
{
	QFont f = font();
	f.setBold(!qFuzzyCompare(value(), GetDefault()));
	setFont(f);
	QDoubleSpinBox::paintEvent(event);
}

double MyDoubleSpinBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		QString type = GetType(objectName());
		if (type == QString("spinbox3") || type == QString("spinboxd3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector3 val = parameterContainer->GetDefault<CVector3>(nameVect);
			defaultValue = val.itemByName(lastChar);
			gotDefault = true;
			setToolTipText();
		}
		else if (type == QString("spinbox4") || type == QString("spinboxd4"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector4 val = parameterContainer->GetDefault<CVector4>(nameVect);
			defaultValue = val.itemByName(lastChar);
			gotDefault = true;
			setToolTipText();
		}
		else
		{
			defaultValue = parameterContainer->GetDefault<double>(parameterName);
			gotDefault = true;
			setToolTipText();
		}
		defaultSingleStep = singleStep();
	}
	return defaultValue;
}

void MyDoubleSpinBox::resetToDefault()
{
	setValue(defaultValue);
	emit valueChanged(defaultValue);
}

QString MyDoubleSpinBox::getDefaultAsString()
{
	return QString("%L1").arg(defaultValue, 0, 'g', 15);
}

QString MyDoubleSpinBox::getFullParameterName()
{
	QString parName = parameterName;
	QString type = GetType(objectName());
	if (type == QString("spinbox3") || type == QString("spinboxd3") || type == QString("spinbox4")
			|| type == QString("spinboxd4"))
		parName = parameterName.left(parameterName.length() - 2);
	return parName;
}

void MyDoubleSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event, lineEdit()->createStandardContextMenu());
}

void MyDoubleSpinBox::focusInEvent(QFocusEvent *event)
{
	QDoubleSpinBox::focusInEvent(event);

	QString type = GetType(objectName());
	if (type != "text")
	{
		if (!slider)
		{
			QWidget *topWidget = window();
			slider = new cFrameSliderPopup(topWidget);
			slider->setFocusPolicy(Qt::NoFocus);

			if (type == QString("spinboxd") || type == QString("spinboxd3")
					|| type == QString("spinboxd4"))
			{
				int dialScale = int(pow(10.0, double(decimals())));
				slider->SetDialMode(dialScale, value());
				hasDial = true;
			}

			slider->hide();
		}

		QWidget *topWidget = window();
		QPoint windowPoint = mapTo(topWidget, QPoint());
		int width = this->width();
		int hOffset = height();
		slider->adjustSize();
		QSize minimumSize = slider->minimumSizeHint();
		width = max(width, int(minimumSize.width() * 0.6));
		slider->setFixedWidth(width);

		if (windowPoint.y() + slider->height() + hOffset > topWidget->height())
			hOffset = -slider->height();

		slider->move(windowPoint.x(), windowPoint.y() + hOffset);
		slider->show();

		setSingleStep(defaultSingleStep);

		connect(slider, SIGNAL(resetPressed()), this, SLOT(slotResetToDefault()));
		connect(slider, SIGNAL(intPressed()), this, SLOT(slotRoundValue()));
		connect(
			slider, &cFrameSliderPopup::changedPrecision, this, &MyDoubleSpinBox::slotChangedPrecision);

		if (hasDial)
		{
			connect(this, SIGNAL(valueChanged(double)), slider, SLOT(slotUpdateValue(double)));
			connect(slider, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
			connect(slider, SIGNAL(upPressed()), this, SLOT(slotZeroValue()));
			connect(slider, SIGNAL(downPressed()), this, SLOT(slot180Value()));
			connect(slider, SIGNAL(rightPressed()), this, SLOT(slot90Value()));
			connect(slider, SIGNAL(leftPressed()), this, SLOT(slotMinus90Value()));
			connect(slider, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()));
			connect(slider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
			connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(slotSliderMoved(int)));
		}
		else
		{
			// connect(slider, SIGNAL(timerTrigger()), this, SLOT(slotSliderTimerUpdateValue()));
			connect(slider, SIGNAL(zeroPressed()), this, SLOT(slotZeroValue()));
			connect(slider, SIGNAL(halfPressed()), this, SLOT(slotHalfValue()));
			connect(slider, SIGNAL(doublePressed()), this, SLOT(slotDoubleValue()));
			connect(slider, SIGNAL(minusPressed()), this, SLOT(slotInvertSign()));
			connect(slider, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()));
			connect(slider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
			connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(slotSliderMoved(int)));
		}
	}
}

void MyDoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
	QDoubleSpinBox::focusOutEvent(event);

	if (slider)
	{
		slider->disconnect();
		slider->hide();
		slider->deleteLater();
		slider = nullptr;
	}
}

void MyDoubleSpinBox::slotSliderTimerUpdateValue()
{
	const double val = value();
	int iDiff = slider->value() - 500;
	if (iDiff != 0)
	{
		double sign = (iDiff > 0) ? 1.0 : -1.0;
		double digits = decimals();
		double dDiff = abs(iDiff) / (600.0 / digits) - digits;
		double change = pow(10.0, dDiff) * sign;
		setValue(val + change);
		emit editingFinished();
	}
}

void MyDoubleSpinBox::slotResetToDefault()
{
	resetToDefault();
}

void MyDoubleSpinBox::slotZeroValue()
{
	setValue(0.0);
}

void MyDoubleSpinBox::slotDoubleValue()
{
	const double val = value();
	setValue(val * 2.0);
}

void MyDoubleSpinBox::slotHalfValue()
{
	const double val = value();
	setValue(val * 0.5);
}

void MyDoubleSpinBox::slotInvertSign()
{
	const double val = value();
	setValue(-val);
}

void MyDoubleSpinBox::slot180Value()
{
	setValue(180.0);
}

void MyDoubleSpinBox::slot90Value()
{
	setValue(90.0);
}

void MyDoubleSpinBox::slotMinus90Value()
{
	setValue(-90.0);
}

void MyDoubleSpinBox::slotRoundValue()
{
	const double val = value();
	setValue(MagicRound(val, 0.05));
}

void MyDoubleSpinBox::slotSliderPressed()
{
	valueBeforeSliderDrag = value();
}

void MyDoubleSpinBox::slotSliderReleased()
{
	emit editingFinished();
}

void MyDoubleSpinBox::slotSliderMoved(int sliderPosition)
{
	double newValue = valueBeforeSliderDrag;

	int iDiff = sliderPosition - 500;

	enumSliderPrecision sliderPrecision = slider->precision();
	double dPrecision = 1.0;
	switch (sliderPrecision)
	{
		case enumSliderPrecision::precisionSuperFine: dPrecision = 3.0; break;
		case enumSliderPrecision::precisionVeryFine: dPrecision = 2.0; break;
		case enumSliderPrecision::precisionFine: dPrecision = 1.0; break;
		case enumSliderPrecision::precisionNormal: dPrecision = 0.0; break;
		case enumSliderPrecision::precisionCoarse: dPrecision = -1.0; break;
	}

	double dDiff = iDiff / 500.0;
	qDebug() << dDiff;
	double sign = (iDiff > 0) ? 1.0 : -1.0;

	if (valueBeforeSliderDrag == 0.0 && !hasDial)
	{
		if (minimum() >= 0.0)
		{
			newValue = pow(10.0, dDiff - dPrecision - 1.0);
		}
		else
		{
			newValue = sign * pow(10.0, fabs(dDiff) - dPrecision - 1.0);
		}
	}
	else
	{
		double change = 0.0;
		if (hasDial)
		{
			change = sign * pow(10.0, fabs(dDiff) - dPrecision - 1.0) * 18.0;
		}
		else
		{
			change = sign * pow(10.0, pow(fabs(dDiff), 0.3) * 10.0 - 10.0 - dPrecision)
							 * fabs(valueBeforeSliderDrag);
		}
		newValue = valueBeforeSliderDrag + change;
	}

	setValue(newValue);

	emit editingFinished();
}

void MyDoubleSpinBox::slotChangedPrecision(enumSliderPrecision _precision)
{
	precision = _precision;
	switch (precision)
	{
		case enumSliderPrecision::precisionSuperFine: setSingleStep(defaultSingleStep * 0.001); break;
		case enumSliderPrecision::precisionVeryFine: setSingleStep(defaultSingleStep * 0.01); break;
		case enumSliderPrecision::precisionFine: setSingleStep(defaultSingleStep * 0.1); break;
		case enumSliderPrecision::precisionNormal: setSingleStep(defaultSingleStep * 1.0); break;
		case enumSliderPrecision::precisionCoarse: setSingleStep(defaultSingleStep * 10.0); break;
	}
}

void MyDoubleSpinBox::UpdateScriptAppearance(bool hasScript)
{
	QFont f = font();
	f.setItalic(hasScript);
	setFont(f);

	if (hasScript)
		setStyleSheet("MyLineEdit {color: yellow;  background-color: blue}");
	else
		setStyleSheet(qobject_cast<QWidget *>(this->parent())->styleSheet());
}
