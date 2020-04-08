/*
 * my_text_edit.cpp
 *
 *  Created on: 30 mar 2020
 *      Author: krzysztof
 */

#include "my_text_edit.h"
#include <QDebug>
#include "highlighter.h"

#include <QtGlobal>

cMyTextEdit::cMyTextEdit(QWidget *parent) : QTextEdit(parent)
{
	QFont usedFont = this->font();
	usedFont.setFamily("Condensed");
	usedFont.setStretch(QFont::Condensed);
	usedFont.setFixedPitch(true);
	setFont(usedFont);

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	QFontMetricsF fm(usedFont);
	setTabStopDistance(fm.horizontalAdvance(' ') * 4.0);
#else
	QFontMetrics fm(usedFont);
	setTabStopWidth(fm.width(' ') * 2);
#endif

	highlighter = new Highlighter(document());
}

cMyTextEdit::~cMyTextEdit()
{
	delete highlighter;
}
