/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * MyTableWidgetKeyframes class -  to display keyframe animation frames
 */

#include "my_table_widget_keyframes.hpp"
#include "../src/animation_keyframes.hpp"
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QtCore>

MyTableWidgetKeyframes::MyTableWidgetKeyframes(QWidget *parent) : QTableWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(tableContextMenuRequest(QPoint)));

	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
		SLOT(columnContextMenuRequest(QPoint)));

	verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->verticalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
		SLOT(rowContextMenuRequest(QPoint)));
}

MyTableWidgetKeyframes::~MyTableWidgetKeyframes()
{
}

void MyTableWidgetKeyframes::tableContextMenuRequest(QPoint point) const
{
	QModelIndex index = indexAt(point);
	int row = index.row();
	int column = index.column();

	QMenu *menu = new QMenu;

	QAction *actionRender = nullptr;
	QAction *interpolateForward = nullptr;

	if (row == 0 && column >= cKeyframeAnimation::reservedColums)
	{
		actionRender = menu->addAction(tr("Render this keyframe"));
	}
	else
	{
		if (column < columnCount() - 1 && column >= cKeyframeAnimation::reservedColums)
		{
			interpolateForward = menu->addAction(tr("Interpolate next keyframes"));
		}
	}

	QAction *selectedItem = menu->exec(viewport()->mapToGlobal(point));

	if (selectedItem)
	{
		if (selectedItem == actionRender)
		{
			gKeyframeAnimation->RenderFrame(column - cKeyframeAnimation::reservedColums);
		}
		else if (selectedItem == interpolateForward)
		{
			gKeyframeAnimation->InterpolateForward(row, column);
		}
	}

	delete menu;
}

void MyTableWidgetKeyframes::columnContextMenuRequest(QPoint point) const
{
	int column = horizontalHeader()->logicalIndexAt(point);

	if (column >= cKeyframeAnimation::reservedColums)
	{
		QMenu *menu = new QMenu;

		QAction *actionRender;
		QAction *actionDelete;
		QAction *actionDeleteTo;
		QAction *actionDeleteFrom;

		actionRender = menu->addAction(tr("Render this keyframe"));
		actionDelete = menu->addAction(tr("Delete this keyframe"));
		actionDeleteTo = menu->addAction(tr("Delete all keyframes to here"));
		actionDeleteFrom = menu->addAction(tr("Delete all keyframes from here"));

		QAction *selectedItem = menu->exec(horizontalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionRender)
			{
				gKeyframeAnimation->RenderFrame(column - cKeyframeAnimation::reservedColums);
			}
			else if (selectedItem == actionDelete)
			{
				gKeyframeAnimation->DeleteKeyframe(column - cKeyframeAnimation::reservedColums);
			}
			else if (selectedItem == actionDeleteFrom)
			{
				gKeyframeAnimation->DeleteFramesFrom(column - cKeyframeAnimation::reservedColums);
			}
			else if (selectedItem == actionDeleteTo)
			{
				gKeyframeAnimation->DeleteFramesTo(column - cKeyframeAnimation::reservedColums);
			}
		}

		delete menu;
	}
}

void MyTableWidgetKeyframes::rowContextMenuRequest(QPoint point) const
{
	int row = verticalHeader()->logicalIndexAt(point);

	QMenu *menu = new QMenu;

	if (row > 0)
	{
		QString name = gKeyframeAnimation->GetParameterName(row);
		QAction *actionDeleteParameter = menu->addAction(tr("Remove '%1' from animation").arg(name));
		menu->addSeparator()->setText(tr("Interpolation type"));
		QAction *actionNoInterpolation = menu->addAction(tr("None"));
		QAction *actionLinearInterpolation = menu->addAction(tr("Linear"));
		QAction *actionLinearAngleInterpolation = menu->addAction(tr("Linear angle"));
		QAction *actionCatMulRomInterpolation = menu->addAction(tr("CatMulRom"));
		QAction *actionCatMulRomAngleInterpolation = menu->addAction(tr("CatMulRom angle"));
		QAction *actionAkimaInterpolation = menu->addAction(tr("Akima"));
		QAction *actionAkimaAngleInterpolation = menu->addAction(tr("Akima angle"));
		QActionGroup actionGroupInterpolation(menu);
		actionGroupInterpolation.addAction(actionNoInterpolation);
		actionGroupInterpolation.addAction(actionLinearInterpolation);
		actionGroupInterpolation.addAction(actionLinearAngleInterpolation);
		actionGroupInterpolation.addAction(actionCatMulRomInterpolation);
		actionGroupInterpolation.addAction(actionCatMulRomAngleInterpolation);
		actionGroupInterpolation.addAction(actionAkimaInterpolation);
		actionGroupInterpolation.addAction(actionAkimaAngleInterpolation);
		actionGroupInterpolation.setExclusive(true);

		actionNoInterpolation->setCheckable(true);
		actionLinearInterpolation->setCheckable(true);
		actionLinearAngleInterpolation->setCheckable(true);
		actionCatMulRomInterpolation->setCheckable(true);
		actionCatMulRomAngleInterpolation->setCheckable(true);
		actionAkimaInterpolation->setCheckable(true);
		actionAkimaAngleInterpolation->setCheckable(true);

		using namespace parameterContainer;
		enumMorphType morphType = gKeyframeAnimation->GetMorphType(row);
		switch (morphType)
		{
			case morphNone: actionNoInterpolation->setChecked(true); break;
			case morphLinear: actionLinearInterpolation->setChecked(true); break;
			case morphLinearAngle: actionLinearAngleInterpolation->setChecked(true); break;
			case morphCatMullRom: actionCatMulRomInterpolation->setChecked(true); break;
			case morphCatMullRomAngle: actionCatMulRomAngleInterpolation->setChecked(true); break;
			case morphAkima: actionAkimaInterpolation->setChecked(true); break;
			case morphAkimaAngle: actionAkimaAngleInterpolation->setChecked(true); break;
		}

		QAction *selectedItem = menu->exec(verticalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionDeleteParameter)
			{
				gKeyframes->RemoveAnimatedParameter(name);
				gKeyframeAnimation->RefreshTable();
			}
			else if (selectedItem == actionNoInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphNone);
			else if (selectedItem == actionLinearInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphLinear);
			else if (selectedItem == actionLinearAngleInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphLinearAngle);
			else if (selectedItem == actionCatMulRomInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphCatMullRom);
			else if (selectedItem == actionCatMulRomAngleInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphCatMullRomAngle);
			else if (selectedItem == actionAkimaInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphAkima);
			else if (selectedItem == actionAkimaAngleInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphAkimaAngle);
		}
	}
	else
	{
		QAction *actionRefreshThumbnails = menu->addAction(tr("Refresh all thumbnails"));

		QAction *selectedItem = menu->exec(verticalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionRefreshThumbnails)
			{
				gKeyframeAnimation->RefreshTable();
			}
		}
	}

	delete menu;
}
