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
 * MyTabBar - promoted QTabBar widget with swap tabs functionality
 */

#include "my_tab_bar.h"
#include "my_check_box.h"
#include <QtWidgets/QtWidgets>

MyTabBar::MyTabBar(QWidget *parent) : QTabBar(parent)
{
	// setMovable (true);
}

void MyTabBar::setupMoveButtons()
{
	for (int i = 0; i < count(); i++)
	{
		QFrame *leftFrame = new QFrame(this);
		QHBoxLayout *leftSide = new QHBoxLayout;
		leftSide->setContentsMargins(0, 0, 0, 0);
		leftSide->setSpacing(2);

		MyCheckBox *chActive = new MyCheckBox(this);
		chActive->setObjectName(QString("checkBox_fractal_enable_%1").arg(i + 1));
		leftSide->addWidget(chActive);
		connect(chActive, SIGNAL(stateChanged(int)), this, SLOT(slotToggleActive(int)));
		chActive->setChecked(i == 0);

		if (i > 0)
		{
			QToolButton *tbMoveLeft = new QToolButton(this);
			QIcon arrowLeft = this->style()->standardIcon(QStyle::SP_ArrowLeft);
			tbMoveLeft->setIcon(arrowLeft);
			tbMoveLeft->setIconSize(QSize(10, 10));
			tbMoveLeft->setObjectName(QString::number(i));
			connect(tbMoveLeft, SIGNAL(clicked()), this, SLOT(slotMoveLeft()));
			leftSide->addWidget(tbMoveLeft);
		}
		leftFrame->setLayout(leftSide);
		setTabButton(i, QTabBar::LeftSide, leftFrame);

		if (i < count() - 1)
		{
			QToolButton *tbMoveRight = new QToolButton(this);
			QIcon arrowRight = this->style()->standardIcon(QStyle::SP_ArrowRight);
			tbMoveRight->setIcon(arrowRight);
			tbMoveRight->setIconSize(QSize(10, 10));
			tbMoveRight->setObjectName(QString::number(i));
			setTabButton(i, QTabBar::RightSide, tbMoveRight);
			connect(tbMoveRight, SIGNAL(clicked()), this, SLOT(slotMoveRight()));
		}
	}
}

void MyTabBar::slotMoveLeft()
{
	QString buttonName = this->sender()->objectName();
	int index = buttonName.toInt();
	if (index == 0) return;
	emit swapTabs(index - 1, index);
}

void MyTabBar::slotMoveRight()
{
	QString buttonName = this->sender()->objectName();
	int index = buttonName.toInt();
	if (index == count() - 1) return;
	emit swapTabs(index, index + 1);
}

void MyTabBar::slotToggleActive(int state)
{

	QString buttonName = this->sender()->objectName();
	int index = buttonName.right(1).toInt() - 1;
	QPalette palette = QApplication::palette();
	setTabTextColor(index, state == Qt::Checked ? palette.text().color() : palette.dark().color());
	// setTabEnabled(index, state == Qt::Checked);
	emit toggledEnable(index + 1, state == Qt::Checked);
}

void MyTabBar::mouseReleaseEvent(QMouseEvent *event)
{
	QTabBar::mouseMoveEvent(event);
	// emit dragDropChange();
}
