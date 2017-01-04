/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * MyTabWidget - promoted QTabWidget with swap tabs functionality
 */

#include "my_tab_widget.h"
#include "my_tab_bar.h"
#include <QPushButton>

MyTabWidget::MyTabWidget(QWidget *parent) : QTabWidget(parent)
{
	MyTabBar *bar = new MyTabBar();
	connect(bar, SIGNAL(swapTabs(int, int)), this, SIGNAL(swapTabs(int, int)));
	this->setTabBar(bar);
	bar->setupMoveButtons();
}

/*
void MyTabWidget::slotDragDropChange()
{
	QRegularExpression re("^.*([0-9])+$");
	for(int l = 0; l < count(); l++)
	{
		QRegularExpressionMatch matchL = re.match(widget(l)->objectName());
		if (!matchL.hasMatch()) return;
		int indexInWidgetL = matchL.captured(1).toInt() - 1;
		if(indexInWidgetL != l)
		{
			for(int r = count() - 1; r >= 0; r--)
			{
				QRegularExpressionMatch matchR = re.match(widget(r)->objectName());
				if (!matchR.hasMatch()) return;
				int indexInWidgetR = matchR.captured(1).toInt() - 1;
				if(indexInWidgetR != r)
				{
					emit swapTabs(l, r);
					return;
				}
			}
		}
	}
}
*/
