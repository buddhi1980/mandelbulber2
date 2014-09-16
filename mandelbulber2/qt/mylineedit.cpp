/*
 * mylineedit.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "mylineedit.h"
#include "../src/parameters.hpp"

void MyLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = createStandardContextMenu();
  menu->addSeparator();
  actionResetToDefault = menu->addAction(tr("Reset to default"));
  QAction *selectedItem = menu->exec(event->globalPos());
  if(selectedItem)
  {
  	if(selectedItem == actionResetToDefault)
  	{
			QString type = GetType(objectName());
  		if(type == QString("vect3"))
  		{
  			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
  			QString nameVect = parameterName.left(parameterName.length() - 2);
  			CVector3 val = parameterContainer->GetDefault<CVector3>(nameVect);
  			QString valS = QString::number(val.itemByName(lastChar), 'g', 20);
  			setText(valS);
  		}
  		else
  		{
  			QString val = parameterContainer->GetDefault<QString>(parameterName);
  			setText(val);
  		}
  		emit editingFinished();

  		//TODO handling of vector type;
  	}
  }
  delete menu;
}

QString MyLineEdit::GetType(const QString &name)
{
	size_t firstDashPosition = name.indexOf("_");
	return name.left(firstDashPosition);
}
