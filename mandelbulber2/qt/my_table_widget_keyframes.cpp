/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <memory>

#include <QActionGroup>
#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>

#include "src/animation_keyframes.hpp"

MyTableWidgetKeyframes::MyTableWidgetKeyframes(QWidget *parent) : QTableWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(tableContextMenuRequest(QPoint)));

	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
		SLOT(columnContextMenuRequest(QPoint)));

	verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(verticalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
		SLOT(rowContextMenuRequest(QPoint)));
}

MyTableWidgetKeyframes::~MyTableWidgetKeyframes() = default;

void MyTableWidgetKeyframes::tableContextMenuRequest(QPoint point) const
{
	QModelIndex index = indexAt(point);
	int row = index.row();
	int column = index.column();

	QList<QTableWidgetItem *> selectedItemsList = selectedItems();

	std::unique_ptr<QMenu> menu(new QMenu);

	QAction *actionRender = nullptr;
	QAction *actionInterpolateForward = nullptr;
	QAction *actionCopyToAll = nullptr;
	QAction *actionMultiplyBy = nullptr;
	QAction *actionIncreaseBy = nullptr;

	if (selectedItemsList.count() > 1)
	{
		actionMultiplyBy = menu->addAction(tr("Multipy values by..."));
		actionIncreaseBy = menu->addAction(tr("Increase values by..."));
	}
	else
	{

		if (row == 0 && column >= cKeyframeAnimation::reservedColumns)
		{
			actionRender = menu->addAction(tr("Render this keyframe"));
		}
		else if (row >= cKeyframeAnimation::reservedRows)
		{
			if (column < columnCount() - 1 && column >= cKeyframeAnimation::reservedColumns)
			{
				actionInterpolateForward = menu->addAction(tr("Interpolate next keyframes"));
			}

			if (column >= cKeyframeAnimation::reservedColumns)
			{
				actionCopyToAll = menu->addAction(tr("Copy value to all keyframes"));
			}
		}
	}

	QAction *selectedAction = menu->exec(viewport()->mapToGlobal(point));

	if (selectedAction)
	{
		if (selectedAction == actionRender)
		{
			gKeyframeAnimation->RenderFrame(column - cKeyframeAnimation::reservedColumns);
		}
		else if (selectedAction == actionInterpolateForward)
		{
			gKeyframeAnimation->InterpolateForward(row, column);
		}
		else if (selectedAction == actionCopyToAll)
		{
			gKeyframeAnimation->CopyToAllKeyframes(row, column);
		}
		if (selectedAction == actionMultiplyBy)
		{
			gKeyframeAnimation->ModifyValueInCells(
				selectedItemsList, cKeyframeAnimation::modifyModeMultiply);
		}
		if (selectedAction == actionIncreaseBy)
		{
			gKeyframeAnimation->ModifyValueInCells(
				selectedItemsList, cKeyframeAnimation::modifyModeIncrease);
		}
	}
}

void MyTableWidgetKeyframes::columnContextMenuRequest(QPoint point) const
{
	int column = horizontalHeader()->logicalIndexAt(point);

	if (column >= cKeyframeAnimation::reservedColumns)
	{
		std::unique_ptr<QMenu> menu(new QMenu);

		QAction *actionRender;
		QAction *actionDelete;
		QAction *actionDeleteTo;
		QAction *actionDeleteFrom;
		QAction *actionInsertInBetween = nullptr;
		QAction *actionDeleteFrames;

		actionRender = menu->addAction(tr("Render this keyframe"));
		actionDelete = menu->addAction(tr("Delete this keyframe"));
		actionDeleteTo = menu->addAction(tr("Delete all keyframes to here"));
		actionDeleteFrom = menu->addAction(tr("Delete all keyframes from here"));

		if (column > cKeyframeAnimation::reservedColumns)
		{
			actionInsertInBetween = menu->addAction(tr("Insert keyframe in between"));
		}

		actionDeleteFrames = menu->addAction(tr("Delete rendered frames for this keyframe"));

		QAction *selectedItem = menu->exec(horizontalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionRender)
			{
				gKeyframeAnimation->RenderFrame(column - cKeyframeAnimation::reservedColumns);
			}
			else if (selectedItem == actionDelete)
			{
				gKeyframeAnimation->DeleteKeyframe(column - cKeyframeAnimation::reservedColumns);
			}
			else if (selectedItem == actionDeleteFrom)
			{
				gKeyframeAnimation->DeleteFramesFrom(column - cKeyframeAnimation::reservedColumns);
			}
			else if (selectedItem == actionDeleteTo)
			{
				gKeyframeAnimation->DeleteFramesTo(column - cKeyframeAnimation::reservedColumns);
			}
			else if (selectedItem == actionInsertInBetween)
			{
				gKeyframeAnimation->InsertKeyframeInBetween(column - cKeyframeAnimation::reservedColumns);
			}
			else if (selectedItem == actionDeleteFrames)
			{
				gKeyframeAnimation->DeleteRenderedFramesForKeyframe(
					column - cKeyframeAnimation::reservedColumns);
			}
		}
	}
}

void MyTableWidgetKeyframes::rowContextMenuRequest(QPoint point) const
{
	int row = verticalHeader()->logicalIndexAt(point);

	std::unique_ptr<QMenu> menu(new QMenu);

	if (row >= cKeyframeAnimation::reservedRows)
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
		QAction *actionCubicInterpolation = menu->addAction(tr("Cubic"));
		QAction *actionCubicAngleInterpolation = menu->addAction(tr("Cubic angle"));
		QAction *actionSteffenInterpolation = menu->addAction(tr("Steffen"));
		QAction *actionSteffenAngleInterpolation = menu->addAction(tr("Steffen angle"));
		QActionGroup actionGroupInterpolation(menu.get());
		actionGroupInterpolation.addAction(actionNoInterpolation);
		actionGroupInterpolation.addAction(actionLinearInterpolation);
		actionGroupInterpolation.addAction(actionLinearAngleInterpolation);
		actionGroupInterpolation.addAction(actionCatMulRomInterpolation);
		actionGroupInterpolation.addAction(actionCatMulRomAngleInterpolation);
		actionGroupInterpolation.addAction(actionAkimaInterpolation);
		actionGroupInterpolation.addAction(actionAkimaAngleInterpolation);
		actionGroupInterpolation.addAction(actionCubicInterpolation);
		actionGroupInterpolation.addAction(actionCubicAngleInterpolation);
		actionGroupInterpolation.addAction(actionSteffenInterpolation);
		actionGroupInterpolation.addAction(actionSteffenAngleInterpolation);
		actionGroupInterpolation.setExclusive(true);

		actionNoInterpolation->setCheckable(true);
		actionLinearInterpolation->setCheckable(true);
		actionLinearAngleInterpolation->setCheckable(true);
		actionCatMulRomInterpolation->setCheckable(true);
		actionCatMulRomAngleInterpolation->setCheckable(true);
		actionAkimaInterpolation->setCheckable(true);
		actionAkimaAngleInterpolation->setCheckable(true);
		actionCubicInterpolation->setCheckable(true);
		actionCubicAngleInterpolation->setCheckable(true);
		actionSteffenInterpolation->setCheckable(true);
		actionSteffenAngleInterpolation->setCheckable(true);

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
			case morphCubic: actionCubicInterpolation->setChecked(true); break;
			case morphCubicAngle: actionCubicAngleInterpolation->setChecked(true); break;
			case morphSteffen: actionSteffenInterpolation->setChecked(true); break;
			case morphSteffenAngle: actionSteffenAngleInterpolation->setChecked(true); break;
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
			else if (selectedItem == actionCubicInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphCubic);
			else if (selectedItem == actionCubicAngleInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphCubicAngle);
			else if (selectedItem == actionSteffenInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphSteffen);
			else if (selectedItem == actionSteffenAngleInterpolation)
				gKeyframeAnimation->ChangeMorphType(row, morphSteffenAngle);
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
}
