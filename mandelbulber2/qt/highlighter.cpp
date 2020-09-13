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
 * Authors: ### TODO: unknown author! ###
 *
 * *************************************************************************
 * *
 * * Copyright (C) 2016 The Qt Company Ltd.
 * * Contact: https://www.qt.io/licensing/
 * *
 * * This file is part of the examples of the Qt Toolkit.
 * *
 * * $QT_BEGIN_LICENSE:BSD$
 * * Commercial License Usage
 * * Licensees holding valid commercial Qt licenses may use this file in
 * * accordance with the commercial license agreement provided with the
 * * Software or, alternatively, in accordance with the terms contained in
 * * a written agreement between you and The Qt Company. For licensing terms
 * * and conditions see https://www.qt.io/terms-conditions. For further
 * * information use the contact form at https://www.qt.io/contact-us.
 * *
 * * BSD License Usage
 * * Alternatively, you may use this file under the terms of the BSD license
 * * as follows:
 * *
 * * "Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions are
 * * met:
 * *   * Redistributions of source code must retain the above copyright
 * *     notice, this list of conditions and the following disclaimer.
 * *   * Redistributions in binary form must reproduce the above copyright
 * *     notice, this list of conditions and the following disclaimer in
 * *     the documentation and/or other materials provided with the
 * *     distribution.
 * *   * Neither the name of The Qt Company Ltd nor the names of its
 * *     contributors may be used to endorse or promote products derived
 * *     from this software without specific prior written permission.
 * *
 * *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 * *
 * * $QT_END_LICENSE$
 * *
 * **************************************************************************
 */

#include "highlighter.h"

#include "src/initparameters.hpp"

//! [0]
Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
	HighlightingRule rule;
	keywordFormat.setForeground(isDarkSkin() ? QColor(137, 142, 255) : Qt::darkBlue);
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "\\bchar\\b"
									<< "\\bclass\\b"
									<< "\\bconst\\b"
									<< "\\bdouble\\b"
									<< "\\benum\\b"
									<< "\\bexplicit\\b"
									<< "\\bfriend\\b"
									<< "\\binline\\b"
									<< "\\bint\\b"
									<< "\\blong\\b"
									<< "\\bnamespace\\b"
									<< "\\boperator\\b"
									<< "\\bprivate\\b"
									<< "\\bprotected\\b"
									<< "\\bpublic\\b"
									<< "\\bshort\\b"
									<< "\\bsignals\\b"
									<< "\\bsigned\\b"
									<< "\\bslots\\b"
									<< "\\bstatic\\b"
									<< "\\bstruct\\b"
									<< "\\btemplate\\b"
									<< "\\btypedef\\b"
									<< "\\btypename\\b"
									<< "\\bunion\\b"
									<< "\\bunsigned\\b"
									<< "\\bvirtual\\b"
									<< "\\bREAL\\b"
									<< "\\bREAL3\\b"
									<< "\\bREAL4\\b"
									<< "\\bfloat\\b"
									<< "\\bfloat2\\b"
									<< "\\bfloat3\\b"
									<< "\\bfloat4\\b"
									<< "\\bvoid\\b"
									<< "\\bvolatile\\b"
									<< "\\bbool\\b";
	foreach (const QString &pattern, keywordPatterns)
	{
		rule.pattern = QRegularExpression(pattern);
		rule.format = keywordFormat;
		highlightingRules.append(rule);
		//! [0] //! [1]
	}
	//! [1]

	//! [2]
	classFormat.setFontWeight(QFont::Bold);
	classFormat.setForeground(isDarkSkin() ? QColor(189, 137, 255) : Qt::darkMagenta);
	rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
	rule.format = classFormat;
	highlightingRules.append(rule);
	//! [2]

	//! [3]
	singleLineCommentFormat.setForeground(Qt::gray);
	rule.pattern = QRegularExpression("//[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::gray);
	//! [3]

	//! [4]
	quotationFormat.setForeground(isDarkSkin() ? QColor(157, 255, 137) : Qt::darkGreen);
	rule.pattern = QRegularExpression("\".*\"");
	rule.format = quotationFormat;
	highlightingRules.append(rule);
	//! [4]

	//! [5]
	functionFormat.setFontItalic(true);
	functionFormat.setForeground(isDarkSkin() ? QColor(137, 142, 255) : Qt::blue);
	rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
	rule.format = functionFormat;
	highlightingRules.append(rule);
	//! [5]

	parameterFormat.setFontWeight(QFont::Bold);
	parameterFormat.setForeground(isDarkSkin() ? QColor(157, 255, 137) : Qt::darkGreen);
	rule.pattern = QRegularExpression("(?:fractal|aux)->(?:[a-zA-Z\\d.]*)\\b");
	rule.format = parameterFormat;
	highlightingRules.append(rule);

	//! [6]
	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");

	// taken from here: https://en.cppreference.com/w/c/keyword
	controlFormat.setForeground(QColor(181, 94, 0)); // orange
	controlFormat.setFontWeight(QFont::Bold);
	QStringList controlKeywordPatterns;
	controlKeywordPatterns << "\\bbreak\\b"
												 << "\\bcase\\b"
												 << "\\bcontinue\\b"
												 << "\\bdefault\\b"
												 << "\\bdo\\b"
												 << "\\belse\\b"
												 << "\\bfor\\b"
												 << "\\bif\\b"
												 << "\\breturn\\b"
												 << "\\bswitch\\b"
												 << "\\bwhile\\b";
	foreach (const QString &pattern, controlKeywordPatterns)
	{
		rule.pattern = QRegularExpression(pattern);
		rule.format = controlFormat;
		highlightingRules.append(rule);
		//! [0] //! [1]
	}
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules)
	{
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext())
		{
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(match.lastCapturedIndex()),
				match.capturedLength(match.lastCapturedIndex()), rule.format);
		}
	}
	//! [7] //! [8]
	setCurrentBlockState(0);
	//! [8]

	//! [9]
	int startIndex = 0;
	if (previousBlockState() != 1) startIndex = text.indexOf(commentStartExpression);

	//! [9] //! [10]
	while (startIndex >= 0)
	{
		//! [10] //! [11]
		QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
		int endIndex = match.capturedStart();
		int commentLength = 0;
		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex + match.capturedLength();
		}
		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
	}
}
//! [11]

bool Highlighter::isDarkSkin()
{
	int skinCode = (gPar->Get<int>("ui_skin"));
	switch (skinCode)
	{
		case 1: // dark skin
		case 4: // Nasa Font dark
		case 5: // Nasa Font dark green
		case 6: // Nasa Font dark blue
		case 7: // Nasa Font dark brown
			return true;
		default: return false;
	}
}
