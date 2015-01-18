/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyUiLoader class - extension for QUiLoader class
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

#include "my_ui_loader.h"
#include "../qt/mylineedit.h"
#include "../qt/myspinbox.h"
#include "../qt/mydoublespinbox.h"
#include "../qt/mycheckbox.h"
#include "../qt/mygroupbox.h"
#include "../qt/mycolorbutton.h"

MyUiLoader::~MyUiLoader()
{
}

QWidget* MyUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
  QWidget* widget = NULL;

  if(className == QString("MyLineEdit"))
  {
  	widget = new MyLineEdit(parent);
  	widget->setObjectName(name);
  }
  else if(className == QString("MySpinBox"))
  {
  	widget = new MySpinBox(parent);
  	widget->setObjectName(name);
  }
  else if(className == QString("MyDoubleSpinBox"))
  {
  	widget = new MyDoubleSpinBox(parent);
  	widget->setObjectName(name);
  }
  else if(className == QString("MyCheckBox"))
  {
  	widget = new MyCheckBox(parent);
  	widget->setObjectName(name);
	}
	else if(className == QString("MyGroupBox"))
	{
		widget = new MyGroupBox(parent);
		widget->setObjectName(name);
	}
	else if(className == QString("MyColorButton"))
	{
		widget = new MyColorButton(parent);
		widget->setObjectName(name);
	}
  else
  {
  	widget = QUiLoader::createWidget(className, parent, name);
	}
  return widget;
}
