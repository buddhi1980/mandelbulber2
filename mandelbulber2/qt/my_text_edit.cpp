/*
 * my_text_edit.cpp
 *
 *  Created on: 30 mar 2020
 *      Author: krzysztof
 */

#include "my_text_edit.h"
#include <QDebug>
#include <QtGlobal>

#include "highlighter.h"
#include "src/system_data.hpp"

cMyTextEdit::cMyTextEdit(QWidget *parent) : QTextEdit(parent)
{
	QFont usedFont = this->font();
	usedFont.setFamily("Condensed");
	usedFont.setStretch(QFont::Condensed);
	usedFont.setPointSize(systemData.getPreferredCustomFormulaFontSize());
	usedFont.setFixedPitch(true);
	setFont(usedFont);

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	QFontMetricsF fm(usedFont);
	setTabStopDistance(fm.horizontalAdvance(' ') * tabSize);
#else
	QFontMetrics fm(usedFont);
	setTabStopWidth(fm.width(' ') * tabSize);
#endif

	highlighter = new Highlighter(document());
}

cMyTextEdit::~cMyTextEdit()
{
	//delete highlighter;
}

void cMyTextEdit::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab)
	{
		// taken from https://github.com/tsujan/FeatherPad/issues/35 GPL 3.0
		QTextCursor cursor = textCursor();
		int newLines = cursor.selectedText().count(QChar::ParagraphSeparator);
		bool isTextSelected = cursor.selectedText().size() > 0;

		cursor.beginEditBlock();
		if (newLines > 0 || isTextSelected)
		{
			if (cursor.anchor() <= cursor.position()) cursor.setPosition(cursor.anchor());

			cursor.movePosition(QTextCursor::StartOfBlock);
		}

		for (int i = 0; i <= newLines; ++i)
		{
			if (e->key() == Qt::Key_Backtab)
			{
				// setCursorPosition(cursor);

				if (cursor.block().text().size() > 0
						&& cursor.block().text().at(cursor.positionInBlock()) == ' ')
				{
					for (int i = 0; i < 4; ++i)
					{
						cursor.deleteChar();
						if (cursor.positionInBlock() % 4 == 0) break;

						if (cursor.positionInBlock() == 0) continue;

						cursor.setPosition(cursor.position() - 1);

						if (cursor.block().text().at(cursor.positionInBlock()) != ' ') break;
					}
				}
				else if (cursor.block().text().size() > 0
								 && cursor.block().text().at(cursor.positionInBlock()) == '\t')
					cursor.deleteChar();
			}
			else
				cursor.insertText("\t");

			if (!isTextSelected) break;

			if (!cursor.movePosition(QTextCursor::NextBlock)) break; // not needed
		}
		cursor.endEditBlock();
		e->accept();
		return;
	}
	else if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
	{
		QTextCursor cursor = textCursor();
		QString line = textCursor().block().text();
		int spacesCount = 0;
		int tabsCount = 0;
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] == QChar::Space)
				spacesCount++;
			else if (line[i] == QChar::Tabulation)
				tabsCount++;
			else
				break;
		}
		if (line.size() > 0)
		{
			int position = textCursor().position();
			QChar ch = toPlainText()[position - 1];
			if (ch == '{')
			{
				tabsCount++;
			}
		}
		int totalSpaces = spacesCount + tabsCount * tabSize;
		int tabsToInsert = totalSpaces / tabSize;
		int spacesToInsert = totalSpaces % tabSize;
		QString tabs(tabsToInsert, QChar::Tabulation);
		QString spaces(spacesToInsert, QChar::Space);
		QTextEdit::keyPressEvent(e);
		insertPlainText(tabs + spaces);
	}
	else
	{
		QTextEdit::keyPressEvent(e);
	}
}

void cMyTextEdit::sendUpdateSignal()
{
	emit signalUpdate();
}
