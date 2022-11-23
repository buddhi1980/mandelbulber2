/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * MySpinBox class - promoted QSpinBox widget with context menu
 */

#include "my_spin_box.h"

#include <QLineEdit>
#include <QMenu>
#include <QWidget>

#include "frame_slider_popup.h"

#include "src/parameters.hpp"

MySpinBox::MySpinBox(QWidget *parent) : QSpinBox(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = 0;
	slider = nullptr;
}

MySpinBox::~MySpinBox()
{
	if (slider) delete slider;
}

void MySpinBox::resetToDefault()
{
	setValue(defaultValue);
	emit valueChanged(defaultValue);
}

QString MySpinBox::getDefaultAsString()
{
	return QString::number(defaultValue);
}

QString MySpinBox::getFullParameterName()
{
	return parameterName;
}

void MySpinBox::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event, lineEdit()->createStandardContextMenu());
}

void MySpinBox::paintEvent(QPaintEvent *event)
{
	QFont f = font();
	f.setBold(value() != GetDefault());
	setFont(f);
	QSpinBox::paintEvent(event);
}

int MySpinBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;
		setToolTipText();
	}
	return defaultValue;
}

void MySpinBox::focusInEvent(QFocusEvent *event)
{
	QSpinBox::focusInEvent(event);

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
		// update min and max
		// slider->SetIntegerMode(minimum(), maximum(), value());

		QWidget *topWidget = window();
		QPoint windowPoint = mapTo(topWidget, QPoint());
		int width = this->width();
		int hOffset = height();

		QSize minimumSize = slider->minimumSizeHint();
		width = qMax(width, int(minimumSize.width() * 0.6));

		slider->adjustSize();
		slider->setFixedWidth(width);

		if (windowPoint.y() + slider->height() + hOffset > topWidget->height())
			hOffset = -slider->height();

		slider->move(windowPoint.x(), windowPoint.y() + hOffset);
		slider->show();

		connect(slider, SIGNAL(resetPressed()), this, SLOT(slotResetToDefault()));
		connect(slider, SIGNAL(zeroPressed()), this, SLOT(slotZeroValue()));
		connect(slider, SIGNAL(halfPressed()), this, SLOT(slotHalfValue()));
		connect(slider, SIGNAL(doublePressed()), this, SLOT(slotDoubleValue()));
		connect(slider, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()));
		connect(slider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
		connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(slotSliderMoved(int)));
	}
}

void MySpinBox::focusOutEvent(QFocusEvent *event)
{
	QSpinBox::focusOutEvent(event);

	if (slider)
	{
		slider->disconnect();
		slider->hide();
		slider->deleteLater();
		slider = nullptr;
	}
}

void MySpinBox::slotResetToDefault()
{
	resetToDefault();
}

void MySpinBox::slotZeroValue()
{
	setValue(0);
}

void MySpinBox::slotDoubleValue()
{
	setValue(value() * 2);
}

void MySpinBox::slotHalfValue()
{
	setValue(value() / 2);
}

void MySpinBox::slotSliderPressed()
{
	valueBeforeSliderDrag = value();
}

void MySpinBox::slotSliderReleased()
{
	emit editingFinished();
}

void MySpinBox::slotSliderMoved(int sliderPosition)
{
	int newValue = valueBeforeSliderDrag;

	int iDiff = sliderPosition - 500;

	enumSliderPrecision precision = slider->precision();
	double dPrecision = 1.0;
	switch (precision)
	{
		case enumSliderPrecision::precisionFine: dPrecision = 0.5; break;
		case enumSliderPrecision::precisionNormal: dPrecision = 1.0; break;
		case enumSliderPrecision::precisionCoarse: dPrecision = 2.0; break;
	}

	double dDiff = iDiff / 500.0 * dPrecision;
	double sign = (iDiff > 0) ? 1.0 : -1.0;
	double digits = log10(double(maximum()));

	if (valueBeforeSliderDrag == 0)
	{
		if (minimum() >= 0.0)
		{
			newValue = pow(10.0, dDiff * (digits + 1.0) - digits);
		}
		else
		{
			newValue = sign * pow(10.0, fabs(dDiff) * (digits + 1.0) - digits);
		}
	}
	else
	{
		double change = sign * pow(10.0, fabs(dDiff) * digits);
		newValue = valueBeforeSliderDrag + change;
	}

	setValue(newValue);

	emit editingFinished();
}
