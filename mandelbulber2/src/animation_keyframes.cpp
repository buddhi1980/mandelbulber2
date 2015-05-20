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
#include "files.h"
#include "error_message.hpp"
#include "progress_text.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include "thumbnail_widget.h"
#include <QInputDialog>

cKeyframeAnimation::cKeyframeAnimation(cInterface *_interface, cKeyframes *_frames, QObject *parent) : QObject(parent), mainInterface(_interface), keyframes(_frames)
{
	ui = mainInterface->mainWindow->ui;
	QApplication::connect(ui->pushButton_record_keyframe, SIGNAL(clicked()), this, SLOT(slotRecordKeyframe()));
	QApplication::connect(ui->pushButton_render_keyframe_animation, SIGNAL(clicked()), this, SLOT(slotRenderKeyframes()));
	QApplication::connect(ui->pushButton_delete_all_keyframe_images, SIGNAL(clicked()), this, SLOT(slotDeleteAllImages()));
	QApplication::connect(ui->pushButton_show_keyframe_animation, SIGNAL(clicked()), this, SLOT(slotShowAnimation()));
	QApplication::connect(ui->pushButton_refresh_keyframe_table, SIGNAL(clicked()), this, SLOT(slotRefreshTable()));

	QApplication::connect(ui->button_selectAnimKeyframeImageDir, SIGNAL(clicked()), this, SLOT(slotSelectKeyframeAnimImageDir()));
	QApplication::connect(ui->tableWidget_keyframe_animation, SIGNAL(cellChanged(int, int)), this, SLOT(slotTableCellChanged(int, int)));

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

void cKeyframeAnimation::slotRecordKeyframe()
{
	if(keyframes)
	{
		//get latest values of all parameters
		mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);

		//add new frame to container
		keyframes->AddFrame(*gPar, *gParFractal);

		//add column to table
		int newColumn = AddColumn(keyframes->GetFrame(keyframes->GetNumberOfFrames() - 1));
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
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

void cKeyframeAnimation::UpdateThumbnailFromImage(int index)
{
	table->blockSignals(true);
	QImage qimage((const uchar*)mainInterface->mainImage->ConvertTo8bit(), mainInterface->mainImage->GetWidth(), mainInterface->mainImage->GetHeight(), mainInterface->mainImage->GetWidth()*sizeof(sRGB8), QImage::Format_RGB888);
	QPixmap pixmap;
	pixmap.convertFromImage(qimage);
	QIcon icon(pixmap.scaled(QSize(100, 70), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
	table->setItem(0, index, new QTableWidgetItem(icon, QString()));
	table->blockSignals(false);
}

void cKeyframeAnimation::PrepareTable()
{
	//manual delete of all cellWidgets
	//FIXME deleting of cell widgets doesn't work properly. QTableWidgets don't free memory when clear() or removeCellWidget is used
	//It calls destructors for cell widgets only wnen QTable widget is destroyed.
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

int cKeyframeAnimation::AddColumn(const cAnimationFrames::sAnimationFrame &frame)
{
	table->blockSignals(true);
	int newColumn = table->columnCount();
	table->insertColumn(newColumn);

	QList<cAnimationFrames::sParameterDescription> parList = keyframes->GetListOfUsedParameters();

	using namespace parameterContainer;
	for(int i=0; i<parList.size(); ++i)
	{
		QString parameterName = parList[i].containerName + "_" + parList[i].parameterName;
		enumVarType type = parList[i].varType;
		int row = parameterRows[i];

		if (type == typeVector3)
		{
			CVector3 val = frame.parameters.Get<CVector3>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString::number(val.x, 'g', 16)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString::number(val.y, 'g', 16)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString::number(val.z, 'g', 16)));
		}
		else if (type == typeRgb)
		{
			sRGB val = frame.parameters.Get<sRGB>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString::number(val.R)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString::number(val.G)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString::number(val.B)));
		}
		else
		{
			QString val = frame.parameters.Get<QString>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(val));
		}
	}
	table->blockSignals(false);
	return newColumn;
}

void cKeyframeAnimation::RenderKeyframes()
{

	//TODO RenderKeyframes() has to be rewrited

	if(mainInterface->mainImage->IsUsed())
	{
		cErrorMessage::showMessage(QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"), cErrorMessage::errorMessage);
		return;
	}

	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);

	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, mainInterface->mainImage, &mainInterface->stopRequest, mainInterface->mainWindow, mainInterface->renderedImage);

	renderJob->Init(cRenderJob::flightAnim);
	mainInterface->stopRequest = false;

	QString framesDir = gPar->Get<QString>("anim_flight_dir");

	mainInterface->progressBarAnimation->show();
	cProgressText progressText;
	progressText.ResetTimer();

	// Check if frames have already been rendered
	for(int index = 0; index < keyframes->GetNumberOfFrames(); ++index)
	{
		QString filename = framesDir + "frame" + QString("%1").arg(index, 5, 10, QChar('0')) + QString(".jpg");
		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);
		frame.alreadyRendered = QFile(filename).exists();
		keyframes->ModifyFrame(index, frame);
	}

	int unrenderedTotal = keyframes->GetUnrenderedTotal();


//	if(frames->GetNumberOfFrames() > 0 && unrenderedTotal == 0){
//		QMessageBox::StandardButton reply;
//		reply = QMessageBox::question(
//			ui->centralwidget,
//			QObject::tr("Truncate Image Folder"),
//			QObject::tr("The animation has already been rendered completely.\n Do you want to purge the output folder?\n")
//			+ QObject::tr("This will delete all images in the image folder.\nProceed?"),
//			QMessageBox::Yes|QMessageBox::No);
//
//		if (reply == QMessageBox::Yes)
//		{
//			DeleteAllFilesFromDirectory(gPar->Get<QString>("anim_flight_dir"));
//			return RenderFlight();
//		}
//		else
//		{
//			return;
//		}
//	}

	for(int index = 0; index < keyframes->GetNumberOfFrames(); ++index)
	{
		double percentDoneFrame = (keyframes->GetUnrenderedTillIndex(index) * 1.0) / unrenderedTotal;
		QString progressTxt = progressText.getText(percentDoneFrame);

		ProgressStatusText(QObject::tr("Animation start"),
			QObject::tr("Frame %1 of %2").arg((index + 1)).arg(keyframes->GetNumberOfFrames()) + " " + progressTxt,
			percentDoneFrame,
			ui->statusbar, mainInterface->progressBarAnimation);

		// Skip already rendered frames
		if(keyframes->GetFrame(index).alreadyRendered)
		{
			//int firstMissing = index;
			while(index < keyframes->GetNumberOfFrames() && keyframes->GetFrame(index).alreadyRendered)
			{
				index++;
			}
			index--;
			continue;
		}

		//-------------- rendering of interpolated keyframes ----------------
		for(int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
		{
			if(mainInterface->stopRequest) break;
			keyframes->GetInterpolatedFrameAndConsolidate(index * keyframes->GetFramesPerKeyframe() + subindex, gPar, gParFractal);
			mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
			renderJob->UpdateParameters(gPar, gParFractal);
			int result = renderJob->Execute();
			if(!result) break;

			QString filename = framesDir + "frame_interpolated" + QString("%1_%2").arg(index, 5, 10, QChar('0')).arg(subindex, 5, 10, QChar('0')) + QString(".jpg");
			SaveJPEGQt(filename, mainInterface->mainImage->ConvertTo8bit(), mainInterface->mainImage->GetWidth(), mainInterface->mainImage->GetHeight(), 95);
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

		if(ui->checkBox_flight_show_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, NULL, table);
			thumbWidget->UseOneCPUCore(true);
			keyframes->GetFrameAndConsolidate(i, &tempPar, &tempFract);
			thumbWidget->AssignParameters(tempPar, tempFract);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		if(i % 100 == 0)
		{
			ProgressStatusText(QObject::tr("Refreshing animation"), tr("Refreshing animation frames"), (double)i / noOfFrames, ui->statusbar, mainInterface->progressBarAnimation);
			gApplication->processEvents();
		}
	}
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
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

	mainInterface->StartRender();
}

void cKeyframeAnimation::DeleteFramesFrom(int index)
{
	keyframes->DeleteFrames(index, keyframes->GetNumberOfFrames() - 1);
	RefreshTable();
}

void cKeyframeAnimation::DeleteFramesTo(int index)
{
	keyframes->DeleteFrames(0, index);
	RefreshTable();
}

void cKeyframeAnimation::slotSelectKeyframeAnimImageDir()
{
	QFileDialog* dialog = new QFileDialog();
	dialog->setFileMode(QFileDialog::DirectoryOnly);
	dialog->setNameFilter(QObject::tr("Animation Image Folder"));
	dialog->setDirectory(gPar->Get<QString>("anim_flight_dir"));
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setWindowTitle(QObject::tr("Choose Animation Image Folder"));
	dialog->setOption(QFileDialog::ShowDirsOnly);
	QStringList filenames;

	if(dialog->exec())
	{
		filenames = dialog->selectedFiles();
		QString filename = filenames.first() + "/";
		ui->text_anim_flight_dir->setText(filename);
		gPar->Set("anim_flight_dir", filename);
	}
}

void cKeyframeAnimation::slotTableCellChanged(int row, int column)
{
	//FIXME program crashes when there is edited (by double click) first row (with previews)
	table->blockSignals(true);
	QTableWidgetItem *cell = ui->tableWidget_flightAnimation->item(row, column);
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
		if(vectIndex == 0) vect.x = cellText.toDouble();
		if(vectIndex == 1) vect.y = cellText.toDouble();
		if(vectIndex == 2) vect.z = cellText.toDouble();
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
	if (ui->checkBox_flight_show_thumbnails->isChecked())
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

void cKeyframeAnimation::slotDeleteAllImages()
{
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_flightAnimationParameters, gPar, cInterface::read);

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(
		ui->centralwidget,
		QObject::tr("Truncate Image Folder"),
		QObject::tr("This will delete all images in the image folder.\nProceed?"),
		QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		DeleteAllFilesFromDirectory(gPar->Get<QString>("anim_flight_dir"));
	}
}

void cKeyframeAnimation::slotShowAnimation()
{

	WriteLog("Prepare PlayerWidget class");
	mainInterface->imageSequencePlayer = new PlayerWidget();
	mainInterface->imageSequencePlayer->show();
}


void cKeyframeAnimation::slotRefreshTable()
{
	RefreshTable();
}
