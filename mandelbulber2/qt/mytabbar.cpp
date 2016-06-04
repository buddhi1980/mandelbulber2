/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyTabBar class - promoted MyTabBar widget with drag and drop functionality
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Sebastian Jennen
 */

#include "mytabbar.h"
#include <QtWidgets/QtWidgets>
#include "../qt/mycheckbox.h"

MyTabBar::MyTabBar(QWidget *parent) :	QTabBar(parent)
{
	// setMovable (true);
}

void MyTabBar::setupMoveButtons()
{
	for(int i = 0; i < count(); i++)
	{
		QFrame *leftFrame = new QFrame(this);
		QHBoxLayout *leftSide = new QHBoxLayout;
		leftSide->setContentsMargins(0, 0, 0, 0);
		leftSide->setSpacing(2);

		MyCheckBox* chActive = new MyCheckBox(this);
		chActive->setObjectName(QString("checkBox_fractal_enable_%1").arg(i + 1));
		leftSide->addWidget(chActive);
		connect(chActive, SIGNAL(stateChanged(int)), this, SLOT(slotToggleActive(int)));
		chActive->setChecked(i == 0);

		if(i > 0)
		{
			QToolButton* tbMoveLeft = new QToolButton(this);
			QIcon arrowLeft = this->style()->standardIcon(QStyle::SP_ArrowLeft);
			tbMoveLeft->setIcon(arrowLeft);
			tbMoveLeft->setIconSize(QSize(10, 10));
			tbMoveLeft->setObjectName(QString::number(i));
			connect(tbMoveLeft, SIGNAL(clicked()), this, SLOT(slotMoveLeft()));
			leftSide->addWidget(tbMoveLeft);
		}
		leftFrame->setLayout(leftSide);
		setTabButton(i, QTabBar::LeftSide, leftFrame);

		if(i < count() - 1)
		{
			QToolButton* tbMoveRight = new QToolButton(this);
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
	if(index == 0) return;
	emit swapTabs(index - 1, index);
}

void MyTabBar::slotMoveRight()
{
	QString buttonName = this->sender()->objectName();
	int index = buttonName.toInt();
	if(index == count() - 1) return;
	emit swapTabs(index, index + 1);
}

void MyTabBar::slotToggleActive(int state){

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
