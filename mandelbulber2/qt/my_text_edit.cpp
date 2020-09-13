/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
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
	// delete highlighter;
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
