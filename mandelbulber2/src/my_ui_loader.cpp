/*
 * MyUILoader.cpp
 *
 *  Created on: Jul 12, 2014
 *      Author: krzysztof
 */

#include "my_ui_loader.h"
#include "../qt/mylineedit.h"
#include "../qt/myspinbox.h"
#include "../qt/mydoublespinbox.h"
#include "../qt/mycheckbox.h"

MyUiLoader::~MyUiLoader()
{
	// TODO Destructor for MyUiLoader
}

QWidget* MyUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
  QWidget* widget = NULL;

  //TODO more different custom widgets

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
  else
  {
  	widget = QUiLoader::createWidget(className, parent, name);
  }
  return widget;
}
