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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cAutomatedWidgets class provides connection between sliders
 * and edit fields. This class should be subclassed in every window or dialog
 * with edit fields and called there ConnectSignalsForSlidersInWindow();
 */

#include "automated_widgets.hpp"

#include <QDoubleSpinBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QtWidgets>

#include "synchronize_interface.hpp"
#include "system.hpp"

cAutomatedWidgets::cAutomatedWidgets(QObject *parent) : QObject(parent)
{
}

void cAutomatedWidgets::slotSliderMoved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinbox_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
	if (spinBox)
	{
		double decimals = spinBox->decimals();
		double divider = pow(10.0, decimals);
		spinBox->setValue(value / divider);
	}
	else
	{
		qWarning() << "slotSliderMoved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotDoubleSpinBoxChanged(double value) const
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("slider_") + parameterName;
	QSlider *slider = this->sender()->parent()->findChild<QSlider *>(sliderName);
	if (slider)
	{
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(this->sender());
		double decimals = spinBox->decimals();
		double multiplier = pow(10.0, decimals);
		slider->setValue(int(value * multiplier));
	}
	else
	{
		qWarning() << "slotDoubleSpinBoxChanged() error: slider " << sliderName << " doesn't exists"
							 << endl;
	}
}

void cAutomatedWidgets::slotIntSliderMoved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString spinboxName = QString("spinboxInt_") + parameterName;

	QSpinBox *spinbox = this->sender()->parent()->findChild<QSpinBox *>(spinboxName);
	if (spinbox)
	{
		spinbox->setValue(value);
	}
	else
	{
		qWarning() << "slotLogSliderMoved() error: lineEdit " << spinboxName << " doesn't exists"
							 << endl;
	}
}

void cAutomatedWidgets::slotIntSpinBoxChanged(int value) const
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("sliderInt_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider *>(sliderName);
	if (slider)
	{
		slider->setValue(value);
	}
	else
	{
		qWarning() << "slotIntSpinBoxChanged() error: slider " << sliderName << " doesn't exists"
							 << endl;
	}
}

void cAutomatedWidgets::slotSlider3Moved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinbox3_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox *>(spinBoxName);

	if (spinBox)
	{
		double decimals = spinBox->decimals();
		double divider = pow(10.0, decimals);
		spinBox->setValue(value / divider);
	}
	else
	{
		qWarning() << "slotSlider3Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotSlider4Moved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinbox4_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox *>(spinBoxName);

	if (spinBox)
	{
		double decimals = spinBox->decimals();
		double divider = pow(10.0, decimals);
		spinBox->setValue(value / divider);
	}
	else
	{
		qWarning() << "slotSlider4Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotSpinBox3Changed(double value) const
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("slider3_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider *>(sliderName);

	if (slider)
	{
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(this->sender());
		double decimals = spinBox->decimals();
		double multiplier = pow(10.0, decimals);
		slider->setValue(int(value * multiplier));
	}
	else
	{
		qWarning() << "slotSpinBox3Changed() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotSpinBox4Changed(double value) const
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("slider4_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider *>(sliderName);

	if (slider)
	{
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(this->sender());
		double decimals = spinBox->decimals();
		double multiplier = pow(10.0, decimals);
		slider->setValue(int(value * multiplier));
	}
	else
	{
		qWarning() << "slotSpinBox4Changed() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotDial3Moved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinboxd3_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
	if (spinBox)
	{
		spinBox->setValue(value / 100.0);
	}
	else
	{
		qWarning() << "slotDial3Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotDial4Moved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinboxd4_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
	if (spinBox)
	{
		spinBox->setValue(value / 100.0);
	}
	else
	{
		qWarning() << "slotDial4Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotSpinBoxD3Changed(double value) const
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(spinBoxName, &parameterName, &type);
	QString dialName = QString("dial3_") + parameterName;

	QDial *dial = this->sender()->parent()->findChild<QDial *>(dialName);
	if (dial)
	{
		dial->setValue(int(value * 100.0));
	}
	else
	{
		qWarning() << "slotSpinBox3Changed() error: slider " << dialName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotSpinBoxD4Changed(double value) const
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(spinBoxName, &parameterName, &type);
	QString dialName = QString("dial4_") + parameterName;

	QDial *dial = this->sender()->parent()->findChild<QDial *>(dialName);
	if (dial)
	{
		dial->setValue(int(value * 100.0));
	}
	else
	{
		qWarning() << "slotSpinBox4Changed() error: slider " << dialName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotDialMoved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinboxd_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
	if (spinBox)
	{
		spinBox->setValue(value / 100.0);
	}
	else
	{
		qWarning() << "slotDialMoved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotSpinBoxDChanged(double value) const
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(spinBoxName, &parameterName, &type);
	QString dialName = QString("dial_") + parameterName;

	QDial *dial = this->sender()->parent()->findChild<QDial *>(dialName);
	if (dial)
	{
		dial->setValue(int(value * 100.0));
	}
	else
	{
		qWarning() << "slotSpinBoxChanged() error: slider " << dialName << " doesn't exists" << endl;
	}
}

void cAutomatedWidgets::slotLogSliderMoved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString lineEditName = QString("logedit_") + parameterName;

	QLineEdit *lineEdit = this->sender()->parent()->findChild<QLineEdit *>(lineEditName);
	if (lineEdit)
	{
		double dValue = pow(10.0, value / 100.0);
		QString text = QString("%L1").arg(dValue);
		lineEdit->setText(text);
	}
	else
	{
		qWarning() << "slotLogSliderMoved() error: lineEdit " << lineEditName << " doesn't exists"
							 << endl;
	}
}

void cAutomatedWidgets::slotLogSliderVect3Moved(int value) const
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(sliderName, &parameterName, &type);
	QString lineEditName = QString("logvect3_") + parameterName;

	QLineEdit *lineEdit = this->sender()->parent()->findChild<QLineEdit *>(lineEditName);
	if (lineEdit)
	{
		double dValue = pow(10.0, value / 100.0);
		QString text = QString("%L1").arg(dValue);
		lineEdit->setText(text);
	}
	else
	{
		qWarning() << "slotLogSlidervect3Moved() error: lineEdit " << lineEditName << " doesn't exists"
							 << endl;
	}
}

void cAutomatedWidgets::slotLogLineEditChanged(const QString &text) const
{
	using namespace std;
	QString lineEditName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(lineEditName, &parameterName, &type);
	QString sliderName = QString("logslider_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider *>(sliderName);
	if (slider)
	{
		double value = systemData.locale.toDouble(text);
		if (value > 0.0)
		{
			int sliderPosition = int(log10(systemData.locale.toDouble(text)) * 100.0);
			slider->setValue(sliderPosition);
		}
		else
		{
			qWarning() << "slotLogLineEditChanged() error: value from " << lineEditName
								 << " is not greater zero" << endl;
		}
	}
	else
	{
		qWarning() << "slotLogLineEditChanged() error: slider " << sliderName << " doesn't exists"
							 << endl;
	}
}

void cAutomatedWidgets::slotLogVect3Changed(const QString &text) const
{
	using namespace std;
	QString lineEditName = this->sender()->objectName();
	QString type, parameterName;
	GetNameAndType(lineEditName, &parameterName, &type);
	QString sliderName = QString("logslidervect3_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider *>(sliderName);
	if (slider)
	{
		double value = systemData.locale.toDouble(text);
		if (value > 0.0)
		{
			int sliderPosition = int(log10(systemData.locale.toDouble(text)) * 100.0);
			slider->setValue(sliderPosition);
		}
		else
		{
			qWarning() << "slotLogVect3Changed() error: value from " << lineEditName
								 << " is not greater zero" << endl;
		}
	}
	else
	{
		qWarning() << "slotLogVect3Changed() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

// NOTE: there are used sliderMoved() signals instead of valueChangd(), because valueChanged()
// caused problems with editing values in edit fields and spinboxes.

// automatic setting of event slots for all sliders
void cAutomatedWidgets::ConnectSignalsForSlidersInWindow(QWidget *window) const
{
	QList<QSlider *> widgetList = window->findChildren<QSlider *>();
	QList<QSlider *>::iterator it;
	for (it = widgetList.begin(); it != widgetList.end(); ++it)
	{
		QString name = (*it)->objectName();
		if (name.length() > 1 && (*it)->metaObject()->className() == QString("QSlider"))
		{
			const QSlider *slider = *it;

			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if (type == QString("slider"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(slotSliderMoved(int)));

				QString spinBoxName = QString("spinbox_") + parameterName;
				QDoubleSpinBox *spinBox = slider->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
				if (spinBox)
				{
					QApplication::connect(
						spinBox, SIGNAL(valueChanged(double)), this, SLOT(slotDoubleSpinBoxChanged(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinbox " << spinBoxName
										 << " doesn't exists" << endl;
				}
			}
			if (type == QString("logslider"))
			{
				QApplication::connect(
					slider, SIGNAL(sliderMoved(int)), this, SLOT(slotLogSliderMoved(int)));

				QString editFieldName = QString("logedit_") + parameterName;
				QLineEdit *lineEdit = slider->parent()->findChild<QLineEdit *>(editFieldName);
				if (lineEdit)
				{
					QApplication::connect(lineEdit, SIGNAL(textChanged(const QString &)), this,
						SLOT(slotLogLineEditChanged(const QString &)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: lineEdit " << editFieldName
										 << " doesn't exists" << endl;
				}
			}
			if (type == QString("logslidervect3"))
			{
				QApplication::connect(
					slider, SIGNAL(sliderMoved(int)), this, SLOT(slotLogSliderVect3Moved(int)));

				QString editFieldName = QString("logvect3_") + parameterName;
				QLineEdit *lineEdit = slider->parent()->findChild<QLineEdit *>(editFieldName);
				if (lineEdit)
				{
					QApplication::connect(lineEdit, SIGNAL(textChanged(const QString &)), this,
						SLOT(slotLogVect3Changed(const QString &)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: lineEdit " << editFieldName
										 << " doesn't exists" << endl;
				}
			}
			if (type == QString("sliderInt"))
			{
				QApplication::connect(
					slider, SIGNAL(sliderMoved(int)), this, SLOT(slotIntSliderMoved(int)));

				QString spinboxName = QString("spinboxInt_") + parameterName;
				QSpinBox *spinbox = slider->parent()->findChild<QSpinBox *>(spinboxName);
				if (spinbox)
				{
					QApplication::connect(
						spinbox, SIGNAL(valueChanged(int)), this, SLOT(slotIntSpinBoxChanged(int)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinboxInt " << spinboxName
										 << " doesn't exists" << endl;
				}
			}
			if (type == QString("slider3"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(slotSlider3Moved(int)));
				QString spinboxName = QString("spinbox3_") + parameterName;
				QDoubleSpinBox *spinbox = slider->parent()->findChild<QDoubleSpinBox *>(spinboxName);
				if (spinbox)
				{
					QApplication::connect(
						spinbox, SIGNAL(valueChanged(double)), this, SLOT(slotSpinBox3Changed(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinbox3 " << spinboxName
										 << " doesn't exists" << endl;
				}
			}
			if (type == QString("slider4"))
			{
				QApplication::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slotSlider4Moved(int)));
				QString spinboxName = QString("spinbox4_") + parameterName;
				QDoubleSpinBox *spinbox = slider->parent()->findChild<QDoubleSpinBox *>(spinboxName);
				if (spinbox)
				{
					QApplication::connect(
						spinbox, SIGNAL(valueChanged(double)), this, SLOT(slotSpinBox4Changed(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinbox4 " << spinboxName
										 << " doesn't exists" << endl;
				}
			}
		}
	}

	QList<QDial *> widgetList2 = window->findChildren<QDial *>(QString());
	QList<QDial *>::iterator it2;
	for (it2 = widgetList2.begin(); it2 != widgetList2.end(); ++it2)
	{
		QString name = (*it2)->objectName();
		if (name.length() > 1 && (*it2)->metaObject()->className() == QString("QDial"))
		{
			const QDial *dial = *it2;
			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if (type == QString("dial3"))
			{
				QApplication::connect(dial, SIGNAL(sliderMoved(int)), this, SLOT(slotDial3Moved(int)));

				QString spinBoxName = QString("spinboxd3_") + parameterName;
				QDoubleSpinBox *spinBox = dial->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
				if (spinBox)
				{
					QApplication::connect(
						spinBox, SIGNAL(valueChanged(double)), this, SLOT(slotSpinBoxD3Changed(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinboxd3 " << spinBoxName
										 << " doesn't exists" << endl;
				}
			}
			if (type == QString("dial4"))
			{
				QApplication::connect(dial, SIGNAL(sliderMoved(int)), this, SLOT(slotDial4Moved(int)));

				QString spinBoxName = QString("spinboxd4_") + parameterName;
				QDoubleSpinBox *spinBox = dial->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
				if (spinBox)
				{
					QApplication::connect(
						spinBox, SIGNAL(valueChanged(double)), this, SLOT(slotSpinBoxD4Changed(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinboxd4 " << spinBoxName
										 << " doesn't exists" << endl;
				}
			}
			if (type == QString("dial"))
			{
				QApplication::connect(dial, SIGNAL(sliderMoved(int)), this, SLOT(slotDialMoved(int)));

				QString spinBoxName = QString("spinboxd_") + parameterName;
				QDoubleSpinBox *spinBox = dial->parent()->findChild<QDoubleSpinBox *>(spinBoxName);
				if (spinBox)
				{
					QApplication::connect(
						spinBox, SIGNAL(valueChanged(double)), this, SLOT(slotSpinBoxDChanged(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinboxd " << spinBoxName
										 << " doesn't exists" << endl;
				}
			}
		}
	}
}
