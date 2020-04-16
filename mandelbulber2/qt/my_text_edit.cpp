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

void cMyTextEdit::keyPressEvent ( QKeyEvent * e ){
	if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab)
	{
		// taken from https://github.com/tsujan/FeatherPad/issues/35 GPL 3.0
		QTextCursor cursor = textCursor();
		int newLines = cursor.selectedText().count (QChar::ParagraphSeparator);
		bool isTextSelected = cursor.selectedText().size() > 0;

		cursor.beginEditBlock();
		if( newLines > 0 || isTextSelected )
		{
				if (cursor.anchor() <= cursor.position())
						cursor.setPosition(cursor.anchor());

				cursor.movePosition (QTextCursor::StartOfBlock);
		}

		for (int i = 0; i <= newLines; ++i)
		{
				if( e->key() == Qt::Key_Backtab )
				{
						// setCursorPosition(cursor);

						if( cursor.block().text().size() > 0 && cursor.block().text().at(cursor.positionInBlock()) == ' ' )
						{
								for (int i = 0; i < 4; ++i)
								{
										cursor.deleteChar();
										if(cursor.positionInBlock() % 4 == 0)
												break;

										if(cursor.positionInBlock() == 0)
												continue;

										cursor.setPosition(cursor.position()-1);

										if( cursor.block().text().at(cursor.positionInBlock()) != ' ' )
												break;
								}
						}
						else if( cursor.block().text().size() > 0 && cursor.block().text().at(cursor.positionInBlock()) == '\t' )
								cursor.deleteChar();
				}
				else
						cursor.insertText ("\t");

				if(!isTextSelected)
						break;

				if (!cursor.movePosition (QTextCursor::NextBlock))
						break; // not needed
		}
		cursor.endEditBlock();
		e->accept();
		return;
	}
	else{
		QTextEdit::keyPressEvent(e);
	}
}
