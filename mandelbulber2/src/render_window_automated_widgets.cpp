/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * RenderWindow class - main program window
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

#include "render_window.hpp"
#include "synchronize_interface.hpp"

void RenderWindow::slotSliderMoved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString spinBoxName = QString("spinbox_") + parameterName;

  QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
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

void RenderWindow::slotDoubleSpinBoxChanged(double value)
{
  using namespace std;
  QString spinBoxName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(spinBoxName, &parameterName, &type);
  QString sliderName = QString("slider_") + parameterName;
  QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
  if (slider)
  {
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(this->sender());
    double decimals = spinBox->decimals();
    double multiplier = pow(10.0, decimals);
    slider->setValue(value * multiplier);
  }
  else
  {
    qWarning() << "slotDoubleSpinBoxChanged() error: slider " << sliderName << " doesn't exists"
        << endl;
  }
}

void RenderWindow::slotIntSliderMoved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString spinboxName = QString("spinboxInt_") + parameterName;

  QSpinBox *spinbox = this->sender()->parent()->findChild<QSpinBox*>(spinboxName);
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

void RenderWindow::slotIntSpinBoxChanged(int value)
{
  using namespace std;
  QString spinBoxName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(spinBoxName, &parameterName, &type);
  QString sliderName = QString("sliderInt_") + parameterName;

  QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
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

void RenderWindow::slotSlider3Moved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString spinBoxName = QString("spinbox3_") + parameterName;

  QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);

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

void RenderWindow::slotSlider4Moved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString spinBoxName = QString("spinbox4_") + parameterName;

  QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);

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

void RenderWindow::slotSpinBox3Changed(double value)
{
  using namespace std;
  QString spinBoxName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(spinBoxName, &parameterName, &type);
  QString sliderName = QString("slider3_") + parameterName;

  QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);

  if (slider)
  {
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(this->sender());
    double decimals = spinBox->decimals();
    double multiplier = pow(10.0, decimals);
    slider->setValue(value * multiplier);
  }
  else
  {
    qWarning() << "slotSpinBox3Changed() error: slider " << sliderName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotSpinBox4Changed(double value)
{
  using namespace std;
  QString spinBoxName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(spinBoxName, &parameterName, &type);
  QString sliderName = QString("slider4_") + parameterName;

  QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);

  if (slider)
  {
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(this->sender());
    double decimals = spinBox->decimals();
    double multiplier = pow(10.0, decimals);
    slider->setValue(value * multiplier);
  }
  else
  {
    qWarning() << "slotSpinBox4Changed() error: slider " << sliderName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotDial3Moved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString spinBoxName = QString("spinboxd3_") + parameterName;

  QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
  if (spinBox)
  {
    spinBox->setValue(value / 100.0);
  }
  else
  {
    qWarning() << "slotDial3Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotDial4Moved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString spinBoxName = QString("spinboxd4_") + parameterName;

  QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
  if (spinBox)
  {
    spinBox->setValue(value / 100.0);
  }
  else
  {
    qWarning() << "slotDial4Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotSpinBoxD3Changed(double value)
{
  using namespace std;
  QString spinBoxName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(spinBoxName, &parameterName, &type);
  QString dialName = QString("dial3_") + parameterName;

  QDial *dial = this->sender()->parent()->findChild<QDial*>(dialName);
  if (dial)
  {
    dial->setValue(value * 100.0);
  }
  else
  {
    qWarning() << "slotSpinBox3Changed() error: slider " << dialName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotSpinBoxD4Changed(double value)
{
  using namespace std;
  QString spinBoxName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(spinBoxName, &parameterName, &type);
  QString dialName = QString("dial4_") + parameterName;

  QDial *dial = this->sender()->parent()->findChild<QDial*>(dialName);
  if (dial)
  {
    dial->setValue(value * 100.0);
  }
  else
  {
    qWarning() << "slotSpinBox4Changed() error: slider " << dialName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotDialMoved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString spinBoxName = QString("spinboxd_") + parameterName;

  QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
  if (spinBox)
  {
    spinBox->setValue(value / 100.0);
  }
  else
  {
    qWarning() << "slotDialMoved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotSpinBoxDChanged(double value)
{
  using namespace std;
  QString spinBoxName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(spinBoxName, &parameterName, &type);
  QString dialName = QString("dial_") + parameterName;

  QDial *dial = this->sender()->parent()->findChild<QDial*>(dialName);
  if (dial)
  {
    dial->setValue(value * 100.0);
  }
  else
  {
    qWarning() << "slotSpinBoxChanged() error: slider " << dialName << " doesn't exists" << endl;
  }
}

void RenderWindow::slotLogSliderMoved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString lineEditName = QString("logedit_") + parameterName;

  QLineEdit *lineEdit = this->sender()->parent()->findChild<QLineEdit*>(lineEditName);
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

void RenderWindow::slotLogSliderVect3Moved(int value)
{
  using namespace std;
  QString sliderName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(sliderName, &parameterName, &type);
  QString lineEditName = QString("logvect3_") + parameterName;

  QLineEdit *lineEdit = this->sender()->parent()->findChild<QLineEdit*>(lineEditName);
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

void RenderWindow::slotLogLineEditChanged(const QString &text)
{
  using namespace std;
  QString lineEditName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(lineEditName, &parameterName, &type);
  QString sliderName = QString("logslider_") + parameterName;

  QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
  if (slider)
  {
    double value = systemData.locale.toDouble(text);
    if (value > 0.0)
    {
      int sliderPosition = log10(systemData.locale.toDouble(text)) * 100.0;
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

void RenderWindow::slotLogVect3Changed(const QString &text)
{
  using namespace std;
  QString lineEditName = this->sender()->objectName();
  QString type, parameterName;
  GetNameAndType(lineEditName, &parameterName, &type);
  QString sliderName = QString("logslidervect3_") + parameterName;

  QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
  if (slider)
  {
    double value = systemData.locale.toDouble(text);
    if (value > 0.0)
    {
      int sliderPosition = log10(systemData.locale.toDouble(text)) * 100.0;
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
    qWarning() << "slotLogVect3Changed() error: slider " << sliderName << " doesn't exists"
        << endl;
  }
}
