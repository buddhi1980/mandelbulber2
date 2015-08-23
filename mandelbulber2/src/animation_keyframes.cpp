/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Functions for flight animation.
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 */

#include "animation_keyframes.hpp"
#include "global_data.hpp"
#include "render_job.hpp"
#include "system.hpp"
#include "error_message.hpp"
#include "progress_text.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include "thumbnail_widget.h"
#include <QInputDialog>
#include "undo.h"

cKeyframeAnimation::cKeyframeAnimation(cInterface *_interface, cKeyframes *_frames, QObject *parent) : QObject(parent), mainInterface(_interface), keyframes(_frames)
{
	ui = mainInterface->mainWindow->ui;
	QApplication::connect(ui->pushButton_add_keyframe, SIGNAL(clicked()), this, SLOT(slotAddKeyframe()));
	QApplication::connect(ui->pushButton_insert_keyframe, SIGNAL(clicked()), this, SLOT(slotInsertKeyframe()));
	QApplication::connect(ui->pushButton_delete_keyframe, SIGNAL(clicked()), this, SLOT(slotDeleteKeyframe()));
	QApplication::connect(ui->pushButton_modify_keyframe, SIGNAL(clicked()), this, SLOT(slotModifyKeyframe()));
	QApplication::connect(ui->pushButton_render_keyframe_animation, SIGNAL(clicked()), this, SLOT(slotRenderKeyframes()));
	QApplication::connect(ui->pushButton_delete_all_keyframe_images, SIGNAL(clicked()), this, SLOT(slotDeleteAllImages()));
	QApplication::connect(ui->pushButton_show_keyframe_animation, SIGNAL(clicked()), this, SLOT(slotShowAnimation()));
	QApplication::connect(ui->pushButton_refresh_keyframe_table, SIGNAL(clicked()), this, SLOT(slotRefreshTable()));
	QApplication::connect(ui->pushButton_keyframe_to_flight_export, SIGNAL(clicked()), this, SLOT(slotExportKeyframesToFlight()));
	QApplication::connect(ui->pushButton_check_for_collisions, SIGNAL(clicked()), this, SLOT(slotValidate()));

	QApplication::connect(ui->button_selectAnimKeyframeImageDir, SIGNAL(clicked()), this, SLOT(slotSelectKeyframeAnimImageDir()));
	QApplication::connect(ui->tableWidget_keyframe_animation, SIGNAL(cellChanged(int, int)), this, SLOT(slotTableCellChanged(int, int)));

	QApplication::connect(ui->spinboxInt_keyframe_first_to_render, SIGNAL(valueChanged(int)), this, SLOT(slotMovedSliderFirstFrame(int)));
	QApplication::connect(ui->spinboxInt_keyframe_last_to_render, SIGNAL(valueChanged(int)), this, SLOT(slotMovedSliderLastFrame(int)));
	QApplication::connect(ui->spinboxInt_frames_per_keyframe, SIGNAL(valueChanged(int)), this, SLOT(UpdateLimitsForFrameRange()));

	table = ui->tableWidget_keyframe_animation;

	//add default parameters for animation
	if (keyframes->GetListOfUsedParameters().size() == 0)
	{
		keyframes->AddAnimatedParameter("camera", gPar->GetAsOneParameter("camera"));
		keyframes->AddAnimatedParameter("target", gPar->GetAsOneParameter("target"));
		keyframes->AddAnimatedParameter("camera_top", gPar->GetAsOneParameter("camera_top"));
		PrepareTable();
	}
}

void cKeyframeAnimation::slotAddKeyframe()
{
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, NULL, keyframes);

	NewKeyframe(keyframes->GetNumberOfFrames());
}

void cKeyframeAnimation::slotInsertKeyframe()
{
	int column = table->currentColumn();
	if(column < 0) column = 0;

	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, NULL, keyframes);

	NewKeyframe(column);
}

void cKeyframeAnimation::NewKeyframe(int index)
{
	if(keyframes)
	{
		//add new frame to container
		keyframes->AddFrame(*gPar, *gParFractal, index);

		gPar->Set("frame_no", keyframes->GetFramesPerKeyframe() * index);

		//add column to table
		int newColumn = AddColumn(keyframes->GetFrame(index), index);
		table->selectColumn(newColumn);

		if(ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, NULL, table);
			thumbWidget->UseOneCPUCore(false);
			thumbWidget->AssignParameters(*gPar, *gParFractal);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		UpdateLimitsForFrameRange();
		ui->spinboxInt_keyframe_last_to_render->setValue(keyframes->GetNumberOfFrames() * ui->spinboxInt_frames_per_keyframe->value());
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cKeyframeAnimation::DeleteKeyframe(int index)
{
	if(index < 0)
	{
		cErrorMessage::showMessage(QObject::tr("No keyframe selected"), cErrorMessage::errorMessage, ui->centralwidget);
	}
	gUndo.Store(gPar, gParFractal, NULL, keyframes);
	keyframes->DeleteFrames(index, index);
	table->removeColumn(index);
	UpdateLimitsForFrameRange();
}

void cKeyframeAnimation::slotModifyKeyframe()
{
	int column = table->currentColumn();

	if(keyframes)
	{
		//get latest values of all parameters
		mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
		gUndo.Store(gPar, gParFractal, NULL, keyframes);

		//add new frame to container
		keyframes->DeleteFrames(column, column);
		keyframes->AddFrame(*gPar, *gParFractal, column);

		//add column to table
		table->removeColumn(column);
		int newColumn = AddColumn(keyframes->GetFrame(column), column);
		table->selectColumn(newColumn);

		if(ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, NULL, table);
			thumbWidget->UseOneCPUCore(false);
			thumbWidget->AssignParameters(*gPar, *gParFractal);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cKeyframeAnimation::slotDeleteKeyframe()
{
	int column = table->currentColumn();
	DeleteKeyframe(column);
}

void cKeyframeAnimation::slotRenderKeyframes()
{
	if(keyframes)
	{
		if(keyframes->GetNumberOfFrames() > 0)
		{
			RenderKeyframes();
		}
		else
		{
			cErrorMessage::showMessage(QObject::tr("No frames to render"), cErrorMessage::errorMessage, ui->centralwidget);
		}

	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cKeyframeAnimation::PrepareTable()
{
	//manual delete of all cellWidgets
	//FIXME deleting of cell widgets doesn't work properly. QTableWidgets don't free memory when clear() or removeCellWidget is used
	//It calls destructors for cell widgets only when QTable widget is destroyed.
	//even if cThumbnailWidget destructors are called, there is still some copy of widget inside the table.

//	for(int i = 1; i < table->columnCount(); i++)
//	{
//		table->removeCellWidget(0, i);
//	}
//	for(int i=0; i<thumbnailWidgets.size(); i++)
//	{
//		qDebug() << thumbnailWidgets[i];
//		delete thumbnailWidgets[i];
//	}
// thumbnailWidgets.clear();

	table->setRowCount(0);
	table->setColumnCount(0);
	table->clear();
	tableRowNames.clear();
	table->verticalHeader()->setDefaultSectionSize(gPar->Get<int>("ui_font_size") + 6);
	CreateRowsInTable();
}

void cKeyframeAnimation::CreateRowsInTable()
{
	QList<cAnimationFrames::sParameterDescription> parList = keyframes->GetListOfUsedParameters();
	table->setIconSize(QSize(100, 70));
	table->insertRow(0);
	table->setVerticalHeaderItem(0, new QTableWidgetItem(tr("preview")));
	table->setRowHeight(0, 70);
	tableRowNames.append(tr("preview"));

	rowParameter.clear();
	rowParameter.append(-1);

	parameterRows.clear();
	for (int i = 0; i < parList.size(); ++i)
	{
		int row = AddVariableToTable(parList[i], i);
		parameterRows.append(row);
	}
}

int cKeyframeAnimation::AddVariableToTable(const cAnimationFrames::sParameterDescription &parameterDescription, int index)
{
	using namespace parameterContainer;
	enumVarType type = parameterDescription.varType;
	int row = table->rowCount();
	if (type == typeVector3)
	{
		QString varName;
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
		tableRowNames.append(varName);
		table->insertRow(row);
		table->setVerticalHeaderItem(row, new QTableWidgetItem(varName));
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_y";
		tableRowNames.append(varName);
		table->insertRow(row + 1);
		table->setVerticalHeaderItem(row + 1, new QTableWidgetItem(varName));
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_z";
		tableRowNames.append(varName);
		table->insertRow(row + 2);
		table->setVerticalHeaderItem(row + 2, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	else if (type == typeRgb)
	{
		QString varName;
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_R";
		tableRowNames.append(varName);
		table->insertRow(row);
		table->setVerticalHeaderItem(row, new QTableWidgetItem(varName));
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_G";
		tableRowNames.append(varName);
		table->insertRow(row + 1);
		table->setVerticalHeaderItem(row + 1, new QTableWidgetItem(varName));
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_B";
		tableRowNames.append(varName);
		table->insertRow(row + 2);
		table->setVerticalHeaderItem(row + 2, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	else
	{
		QString varName = parameterDescription.containerName + "_" + parameterDescription.parameterName;
		tableRowNames.append(varName);
		table->insertRow(table->rowCount());
		table->setVerticalHeaderItem(table->rowCount() - 1, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	return row;
}

int cKeyframeAnimation::AddColumn(const cAnimationFrames::sAnimationFrame &frame, int index)
{
	table->blockSignals(true);
	int newColumn = index;
	if(index == -1) newColumn =	table->columnCount();
	table->insertColumn(newColumn);

	QList<cAnimationFrames::sParameterDescription> parList = keyframes->GetListOfUsedParameters();

	using namespace parameterContainer;
	for(int i=0; i<parList.size(); ++i)
	{
		QString parameterName = parList[i].containerName + "_" + parList[i].parameterName;
		enumVarType type = parList[i].varType;
		int row = parameterRows[i];

		cOneParameter parameter = frame.parameters.GetAsOneParameter(parameterName);
		parameterContainer::enumMorphType morphType = parameter.GetMorphType();

		if (type == typeVector3)
		{
			CVector3 val = parameter.Get<CVector3>(valueActual);
			table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 16)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 16)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 16)));
			table->item(row, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 1, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 2, newColumn)->setBackgroundColor(MorphType2Color(morphType));
		}
		else if (type == typeRgb)
		{
			sRGB val = parameter.Get<sRGB>(valueActual);
			table->setItem(row, newColumn, new QTableWidgetItem(QString::number(val.R)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString::number(val.G)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString::number(val.B)));
			table->item(row, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 1, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 2, newColumn)->setBackgroundColor(MorphType2Color(morphType));
		}
		else
		{
			QString val = parameter.Get<QString>(valueActual);
			table->setItem(row, newColumn, new QTableWidgetItem(val));
			table->item(row, newColumn)->setBackgroundColor(MorphType2Color(morphType));
		}
	}
	table->blockSignals(false);
	return newColumn;
}

QColor cKeyframeAnimation::MorphType2Color(parameterContainer::enumMorphType morphType)
{
	using namespace parameterContainer;
	QColor color;
	switch(morphType)
	{
		case morphNone:
			color = QColor(255, 255, 255);
			break;
		case morphLinear:
			color = QColor(200, 200, 200);
			break;
		case morphLinearAngle:
			color = QColor(200, 200, 255);
			break;
		case morphCatMullRom:
			color = QColor(200, 255, 200);
			break;
		case morphCatMullRomAngle:
			color = QColor(200, 255, 255);
			break;
		case morphAkima:
			color = QColor(255, 200, 200);
			break;
		case morphAkimaAngle:
			color = QColor(255, 200, 255);
			break;
	}
	return color;
}

void cKeyframeAnimation::RenderKeyframes()
{
	if(mainInterface->mainImage->IsUsed())
	{
		cErrorMessage::showMessage(QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"), cErrorMessage::errorMessage);
		return;
	}

	//updating parameters
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, NULL, keyframes);

	keyframes->SetFramesPerKeyframe(gPar->Get<int>("frames_per_keyframe"));

	//checking for collisions
	if(gPar->Get<bool>("keyframe_auto_validate"))
	{
		QList<int> listOfCollisions = CheckForCollisions(gPar->Get<double>("keyframe_collision_thresh"));
		if(listOfCollisions.size() > 0)
		{
			QString collisionText;
			for(int i = 0; i < listOfCollisions.size(); i++)
			{
				collisionText += QString("%1").arg(listOfCollisions.at(i));
				if(i < listOfCollisions.size() - 1) collisionText += QString(", ");
			}
			cErrorMessage::showMessage(QObject::tr("Camera collides with fractal at folowing frames:\n") + collisionText, cErrorMessage::warningMessage);
		}
	}

	//preparing Render Job
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, mainInterface->mainImage, &mainInterface->stopRequest, mainInterface->mainWindow, mainInterface->renderedImage);

	renderJob->Init(cRenderJob::flightAnim);
	mainInterface->stopRequest = false;

	//destination for frames
	QString framesDir = gPar->Get<QString>("anim_keyframe_dir");

	//prepare progress bar for animation
	mainInterface->progressBarAnimation->show();
	cProgressText progressText;
	progressText.ResetTimer();

	//range of keyframes to render
	int startFrame = gPar->Get<int>("keyframe_first_to_render");
	int endFrame = gPar->Get<int>("keyframe_last_to_render");

	// Check if frames have already been rendered
	for(int index = 0; index < keyframes->GetNumberOfFrames(); ++index)
	{
		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);
		frame.alreadyRenderedSubFrames.clear();
		for(int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
		{
			QString filename = GetKeyframeFilename(index, subindex);
			int frameNo = index * keyframes->GetFramesPerKeyframe() + subindex;
			frame.alreadyRenderedSubFrames.append(QFile(filename).exists() || frameNo < startFrame || frameNo >= endFrame);
		}
		keyframes->ModifyFrame(index, frame);
	}
	int unrenderedTotal = keyframes->GetUnrenderedTotal();

	//message if all frames are already rendered
	if(keyframes->GetNumberOfFrames() > 0 && unrenderedTotal == 0){
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			ui->centralwidget,
			QObject::tr("Truncate Image Folder"),
			QObject::tr("The animation has already been rendered completely.\n Do you want to purge the output folder?\n")
			+ QObject::tr("This will delete all images in the image folder.\nProceed?"),
			QMessageBox::Yes|QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			DeleteAllFilesFromDirectory(gPar->Get<QString>("anim_keyframe_dir"), "frame_?????.*");
			return RenderKeyframes();
		}
		else
		{
			return;
		}
	}

	//total number of frames
	int totalFrames = keyframes->GetNumberOfFrames() * keyframes->GetFramesPerKeyframe();

	//main loop for rendering of frames
	for(int index = 0; index < keyframes->GetNumberOfFrames(); ++index)
	{
		//-------------- rendering of interpolated keyframes ----------------
		for(int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
		{
			// skip already rendered frame
			if(keyframes->GetFrame(index).alreadyRenderedSubFrames[subindex])
			{
				continue;
			}

			int frameIndex = index * keyframes->GetFramesPerKeyframe() + subindex;

			double percentDoneFrame = (keyframes->GetUnrenderedTillIndex(frameIndex) * 1.0) / unrenderedTotal;
			QString progressTxt = progressText.getText(percentDoneFrame);

			ProgressStatusText(QObject::tr("Rendering animation"),
				QObject::tr("Frame %1 of %2").arg((frameIndex + 1)).arg(totalFrames) + " " + progressTxt,
				percentDoneFrame,
				ui->statusbar, mainInterface->progressBarAnimation);

			if(mainInterface->stopRequest) break;
			keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, gPar, gParFractal);

			//recalculation of camera rotation and distance (just for display purposes)
			CVector3 camera = gPar->Get<CVector3>("camera");
			CVector3 target = gPar->Get<CVector3>("target");
			CVector3 top = gPar->Get<CVector3>("camera_top");
			cCameraTarget cameraTarget(camera, target, top);
			gPar->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
			gPar->Set("camera_distance_to_target", cameraTarget.GetDistance());

			mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

			//show distance in statistics table
			double distance = mainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"), gPar, gParFractal);
			ui->tableWidget_statistics->item(4, 0)->setText(QString::number(distance));

			if(gNetRender->IsServer())
			{
				gNetRender->WaitForAllClientsReady(2.0);
			}

			gPar->Set("frame_no", frameIndex);
			renderJob->UpdateParameters(gPar, gParFractal);
			int result = renderJob->Execute();
			if(!result) break;
			QString filename = GetKeyframeFilename(index, subindex);
			SaveImage(filename, (enumImageFileType)gPar->Get<int>("keyframe_animation_image_type"), gMainInterface->mainImage);
		}
		//--------------------------------------------------------------------

	}
	ProgressStatusText(QObject::tr("Animation finished"), progressText.getText(1.0), 1.0, ui->statusbar, mainInterface->progressBarAnimation);
}

void cKeyframeAnimation::RefreshTable()
{
	mainInterface->progressBarAnimation->show();
	PrepareTable();
	gApplication->processEvents();

	int noOfFrames = keyframes->GetNumberOfFrames();

	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	cParameterContainer tempPar = *gPar;
	cFractalContainer tempFract = *gParFractal;

	for(int i=0; i < noOfFrames; i++)
	{
		int newColumn = AddColumn(keyframes->GetFrame(i));

		if(ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, NULL, table);
			thumbWidget->UseOneCPUCore(true);
			keyframes->GetFrameAndConsolidate(i, &tempPar, &tempFract);
			tempPar.Set("frame_no", keyframes->GetFramesPerKeyframe() * i);
			thumbWidget->AssignParameters(tempPar, tempFract);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		if(i % 100 == 0)
		{
			ProgressStatusText(QObject::tr("Refreshing animation"), tr("Refreshing animation frames"), (double)i / noOfFrames, ui->statusbar, mainInterface->progressBarAnimation);
			gApplication->processEvents();
		}
	}
	UpdateLimitsForFrameRange();
	mainInterface->progressBarAnimation->hide();
}

QString cKeyframeAnimation::GetParameterName(int rowNumber)
{
	int parameterNumber = rowParameter[rowNumber];

	QString fullParameterName;
	QList<cAnimationFrames::sParameterDescription> list = keyframes->GetListOfUsedParameters();
	if(parameterNumber >= 0)
	{
		fullParameterName = list[parameterNumber].containerName + "_" + list[parameterNumber].parameterName;
	}
	else
	{
		qCritical() << "cFlightAnimation::GetParameterNumber(int rowNumber): row not found";
	}
	return fullParameterName;
}

void cKeyframeAnimation::RenderFrame(int index)
{
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	keyframes->GetFrameAndConsolidate(index, gPar, gParFractal);

	//recalculation of camera rotation and distance (just for display purposes)
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 top = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, top);
	gPar->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
	gPar->Set("camera_distance_to_target", cameraTarget.GetDistance());

	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

	gPar->Set("frame_no", keyframes->GetFramesPerKeyframe() * index);

	mainInterface->StartRender();
}

void cKeyframeAnimation::DeleteFramesFrom(int index)
{
	gUndo.Store(gPar, gParFractal, NULL, keyframes);
	for(int i = keyframes->GetNumberOfFrames() - 1; i >= index; i--) table->removeColumn(index);
	keyframes->DeleteFrames(index, keyframes->GetNumberOfFrames() - 1);
	UpdateLimitsForFrameRange();
}

void cKeyframeAnimation::DeleteFramesTo(int index)
{
	gUndo.Store(gPar, gParFractal, NULL, keyframes);
	for(int i = 0; i <= index; i++) table->removeColumn(0);
	keyframes->DeleteFrames(0, index);
	UpdateLimitsForFrameRange();
}

void cKeyframeAnimation::slotSelectKeyframeAnimImageDir()
{
	QFileDialog* dialog = new QFileDialog();
	dialog->setFileMode(QFileDialog::DirectoryOnly);
	dialog->setNameFilter(QObject::tr("Animation Image Folder"));
	dialog->setDirectory(gPar->Get<QString>("anim_keyframe_dir"));
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setWindowTitle(QObject::tr("Choose Animation Image Folder"));
	dialog->setOption(QFileDialog::ShowDirsOnly);
	QStringList filenames;

	if(dialog->exec())
	{
		filenames = dialog->selectedFiles();
		QString filename = filenames.first() + "/";
		ui->text_anim_keyframe_dir->setText(filename);
		gPar->Set("anim_keyframe_dir", filename);
	}
}

void cKeyframeAnimation::slotTableCellChanged(int row, int column)
{
	if(row > 0)
	{
		table->blockSignals(true);
		QTableWidgetItem *cell = table->item(row, column);
		QString cellText = cell->text();

		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(column);

		QString parameterName = GetParameterName(row);
		int parameterFirstRow = parameterRows[rowParameter[row]];
		int vectIndex = row - parameterFirstRow;

		using namespace parameterContainer;
		enumVarType type = frame.parameters.GetVarType(parameterName);

		if(type == typeVector3)
		{
			CVector3 vect = frame.parameters.Get<CVector3>(parameterName);
			if(vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if(vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if(vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if(type == typeRgb)
		{
			sRGB col = frame.parameters.Get<sRGB>(parameterName);
			if(vectIndex == 0) col.R = cellText.toInt();
			if(vectIndex == 1) col.G = cellText.toInt();
			if(vectIndex == 2) col.B = cellText.toInt();
			frame.parameters.Set(parameterName, col);
		}
		else
		{
			frame.parameters.Set(parameterName, cellText);
		}

		keyframes->ModifyFrame(column, frame);

		//update thumbnail
		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			cParameterContainer tempPar = *gPar;
			cFractalContainer tempFract = *gParFractal;
			keyframes->GetFrameAndConsolidate(column, &tempPar, &tempFract);
			cThumbnailWidget *thumbWidget = (cThumbnailWidget*) table->cellWidget(0, column);

			if (!thumbWidget)
			{
				cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, NULL, table);
				thumbWidget->UseOneCPUCore(true);
				thumbWidget->AssignParameters(tempPar, tempFract);
				table->setCellWidget(0, column, thumbWidget);
			}
			else
			{
				thumbWidget->AssignParameters(tempPar, tempFract);
			}
		}

		table->blockSignals(false);
	}
}

void cKeyframeAnimation::slotDeleteAllImages()
{
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_keyframeAnimationParameters, gPar, cInterface::read);

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(
		ui->centralwidget,
		QObject::tr("Truncate Image Folder"),
		QObject::tr("This will delete all images in the image folder.\nProceed?"),
		QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		DeleteAllFilesFromDirectory(gPar->Get<QString>("anim_keyframe_dir"), "frame_?????.*");
	}
}

void cKeyframeAnimation::slotShowAnimation()
{
	WriteLog("Prepare PlayerWidget class");
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_keyframeAnimationParameters, gPar, cInterface::read);
	mainInterface->imageSequencePlayer = new PlayerWidget();
	mainInterface->imageSequencePlayer->SetFilePath(gPar->Get<QString>("anim_keyframe_dir"));
	mainInterface->imageSequencePlayer->show();
}

void cKeyframeAnimation::InterpolateForward(int row, int column)
{
	gUndo.Store(gPar, gParFractal, NULL, keyframes);

	QTableWidgetItem *cell = table->item(row, column);
	QString cellText = cell->text();

	QString parameterName = GetParameterName(row);

	cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(column);

	using namespace parameterContainer;
	enumVarType type = frame.parameters.GetVarType(parameterName);

	bool valueIsInteger = false;
	bool valueIsDouble = false;
	bool valueIsText = false;
	int valueInteger = 0;
	double valueDouble = 0.0;
	QString valueText;

	bool ok;
	int lastFrame = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation", "Enter last frame number",
			column + 1, column + 2, keyframes->GetNumberOfFrames(), 1, &ok);
	if(!ok) return;

	int numberOfFrames = (lastFrame - column  - 1);

	switch(type)
	{
		case typeBool:
		case typeInt:
		case typeRgb:
		{
			valueIsInteger = true;
			valueInteger = cellText.toInt();
			//qDebug() << valueInteger;
			break;
		}
		case typeDouble:
		case typeVector3:
		{
			valueIsDouble = true;
			valueDouble = systemData.locale.toDouble(cellText);
			//qDebug() << valueDouble;
			break;
		}
		default:
		{
			valueIsText = true;
			valueText = cellText;
			break;
		}
	}

	int finalInteger = 0;
	double finalDouble = 0.0;
	double integerStep = 0.0;
	double doubleStep = 0.0;

	if(valueIsInteger)
	{
		finalInteger = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation", "Enter value for last frame",
				valueInteger, 0, 2147483647, 1, &ok);
		integerStep = (double)(finalInteger - valueInteger) / numberOfFrames;
	}
	else if(valueIsDouble)
	{
		finalDouble = systemData.locale.toDouble(QInputDialog::getText(mainInterface->mainWindow, "Parameter interpolation", "Enter value for last frame", QLineEdit::Normal,
				QString("%L1").arg(valueDouble, 0, 'g', 16), &ok));
		doubleStep = (finalDouble - valueDouble) / numberOfFrames;
	}

	if(!ok) return;

	for(int i = column; i < lastFrame; i++)
	{
		QString newCellText;
		if(valueIsInteger)
		{
			int newValue = integerStep * i + valueInteger;
			newCellText = QString::number(newValue);
		}
		else if(valueIsDouble)
		{
			double newValue = doubleStep * (i - column) + valueDouble;
			newCellText = QString("%L1").arg(newValue, 0, 'g', 16);
		}
		else if(valueIsText)
		{
			newCellText = valueText;
		}
		QTableWidgetItem *newCell = table->item(row, i);
		newCell->setText(newCellText);
	}
}

void cKeyframeAnimation::slotRefreshTable()
{
	RefreshTable();
}

QString cKeyframeAnimation::GetKeyframeFilename(int index, int subindex)
{
	int frameIndex = index * keyframes->GetFramesPerKeyframe() + subindex;
	QString filename = gPar->Get<QString>("anim_keyframe_dir") + "frame_" + QString("%1").arg(frameIndex, 5, 10, QChar('0'));
	switch((enumImageFileType)gPar->Get<double>("keyframe_animation_image_type"))
	{
		case IMAGE_FILE_TYPE_JPG:
			filename += QString(".jpg");
		break;
		case IMAGE_FILE_TYPE_PNG:
			filename += QString(".png");
		break;
		case IMAGE_FILE_TYPE_EXR:
			filename += QString(".exr");
		break;
	}
	return filename;
}

parameterContainer::enumMorphType cKeyframeAnimation::GetMorphType(int row)
{
	parameterContainer::enumMorphType morphType;
	int parameterIndex = rowParameter.at(row);
	morphType = keyframes->GetListOfParameters().at(parameterIndex).morphType;
	return morphType;
}

void cKeyframeAnimation::ChangeMorphType(int row, parameterContainer::enumMorphType morphType)
{
	gUndo.Store(gPar, gParFractal, NULL, keyframes);
	int parameterIndex = rowParameter.at(row);
	keyframes->ChangeMorphType(parameterIndex, morphType);
	RefreshTable();
}

void cKeyframeAnimation::slotExportKeyframesToFlight()
{
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, gAnimFrames, keyframes);
	keyframes->SetFramesPerKeyframe(gPar->Get<int>("frames_per_keyframe"));

	if(gAnimFrames->GetFrames().size() > 0)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
		ui->centralwidget,
		QObject::tr("Export keyframes to flight"),
		QObject::tr("There are already captured flight frames present.\nDiscard current flight frames ?"),
		QMessageBox::Yes|QMessageBox::No);

		if (reply == QMessageBox::No) return;
	}

	gAnimFrames->ClearAll();
	gAnimFrames->SetListOfParametersAndClear(gKeyframes->GetListOfParameters());

	for(int index = 0; index < keyframes->GetNumberOfFrames() - 1; ++index)
	{
		for(int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
		{
			int frameIndex = index * keyframes->GetFramesPerKeyframe() + subindex;
			gAnimFrames->AddFrame(keyframes->GetInterpolatedFrame(frameIndex));
		}
		if(index % 10 == 0)
		{
			ProgressStatusText(QObject::tr("Exporting"), tr("Exporting keyframes to flight"), (double)index / keyframes->GetNumberOfFrames(), ui->statusbar, mainInterface->progressBarAnimation);
			gApplication->processEvents();
		}
	}
	mainInterface->progressBarAnimation->hide();
	ui->tabWidgetFlightKeyframe->setCurrentIndex(0);
	ui->pushButton_flight_refresh_table->animateClick();
}

void cKeyframeAnimation::UpdateLimitsForFrameRange(void)
{
	int framesPerKey = ui->spinboxInt_frames_per_keyframe->value();
	int noOfFrames = keyframes->GetNumberOfFrames() * framesPerKey;

	ui->spinboxInt_keyframe_first_to_render->setMaximum(noOfFrames);
	ui->sliderInt_keyframe_first_to_render->setMaximum(noOfFrames);

	ui->spinboxInt_keyframe_last_to_render->setMaximum(noOfFrames);
	ui->sliderInt_keyframe_last_to_render->setMaximum(noOfFrames);
}

void cKeyframeAnimation::slotMovedSliderFirstFrame(int value)
{
	if(value > ui->spinboxInt_keyframe_last_to_render->value())
		ui->spinboxInt_keyframe_last_to_render->setValue(value);
}
void cKeyframeAnimation::slotMovedSliderLastFrame(int value)
{
	if(value < ui->spinboxInt_keyframe_first_to_render->value())
		ui->spinboxInt_keyframe_first_to_render->setValue(value);
}

QList<int> cKeyframeAnimation::CheckForCollisions(double minDist)
{
	QList<int> listOfCollisions;
	cParameterContainer tempPar = *gPar;
	cFractalContainer tempFractPar = *gParFractal;

	for(int key = 0; key < keyframes->GetNumberOfFrames(); key++)
	{
		ProgressStatusText(QObject::tr("Checking for collissions"),
				QObject::tr("Checking for collissions on keyframe # %1").arg(key),
				(double)key / (keyframes->GetNumberOfFrames() - 1.0),
				ui->statusbar, mainInterface->progressBar);

		for(int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
		{
			int frameIndex = key * keyframes->GetFramesPerKeyframe() + subindex;
			keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, &tempPar, &tempFractPar);
			tempPar.Set("frame_no", frameIndex);
			CVector3 point = tempPar.Get<CVector3>("camera");
			double dist = mainInterface->GetDistanceForPoint(point, &tempPar, &tempFractPar);
			if(dist < minDist)
			{
				listOfCollisions.append(frameIndex);
			}
		}
	}
	ProgressStatusText(QObject::tr("Checking for collissions"), QObject::tr("Checking for collisions finished"), 1.0, ui->statusbar, mainInterface->progressBar);

	return listOfCollisions;
}

void cKeyframeAnimation::slotValidate()
{
	//updating parameters
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, NULL, keyframes);

	keyframes->SetFramesPerKeyframe(gPar->Get<int>("frames_per_keyframe"));

	//checking for collisions
	QList<int> listOfCollisions = CheckForCollisions(gPar->Get<double>("keyframe_collision_thresh"));
	if(listOfCollisions.size() > 0)
	{
		QString collisionText;
		for(int i = 0; i < listOfCollisions.size(); i++)
		{
			collisionText += QString("%1").arg(listOfCollisions.at(i));
			if(i < listOfCollisions.size() - 1) collisionText += QString(", ");
		}
		cErrorMessage::showMessage(QObject::tr("Camera collides with fractal at folowing frames:\n") + collisionText, cErrorMessage::warningMessage);
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("No collisions detected\n"), cErrorMessage::infoMessage);
	}

}
