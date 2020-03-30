/*
 * my_text_edit.cpp
 *
 *  Created on: 30 mar 2020
 *      Author: krzysztof
 */

#include "my_text_edit.h"
#include <QDebug>
#include "highlighter.h"

cMyTextEdit::cMyTextEdit(QWidget *parent) : QTextEdit(parent)
{
	QFont usedFont = this->font();
	double fontSize = usedFont.pointSizeF();
	usedFont.setFamily("Courier");
	usedFont.setFixedPitch(true);
	QFontMetricsF fm(usedFont);
	setFont(usedFont);
	setTabStopDistance(fm.horizontalAdvance(' ') * 2.0);

  highlighter = new Highlighter(document());
}

cMyTextEdit::~cMyTextEdit()
{
	// TODO Auto-generated destructor stub
}
