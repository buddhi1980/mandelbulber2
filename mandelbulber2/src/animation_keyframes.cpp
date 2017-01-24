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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * Functions for flight animation.
 *
 * cKeyframeAnimation contains all functionality for editing and rendering of
 * a keyframe animation. The class holds a cKeyframes to store
 * the parameters of the keyframes and a table to display the frames in a widget.
 * The class can render interpolated subframes and validate their path (check
 * for collision). It exposes slots to manipulate the keyframes.
 */

#include "animation_keyframes.hpp"

#include "../qt/my_progress_bar.h"
#include "../qt/my_table_widget_keyframes.hpp"
#include "../qt/player_widget.hpp"
#include "../qt/system_tray.hpp"
#include "../qt/thumbnail_widget.h"
#include "../qt/pushbutton_anim_sound.h"
#include "../src/render_window.hpp"
#include "cimage.hpp"
#include "dock_animation.h"
#include "dock_statistics.h"
#include "files.h"
#include "global_data.hpp"
#include "headless.h"
#include "interface.hpp"
#include "netrender.hpp"
#include "render_job.hpp"
#include "rendering_configuration.hpp"
#include "ui_dock_animation.h"
#include "undo.h"
#include "common_math.h"

cKeyframeAnimation *gKeyframeAnimation = nullptr;

cKeyframeAnimation::cKeyframeAnimation(cInterface *_interface, cKeyframes *_frames, cImage *_image,
	QWidget *_imageWidget, cParameterContainer *_params, cFractalContainer *_fractal, QObject *parent)
		: QObject(parent), mainInterface(_interface), keyframes(_frames)
{
	image = _image;
	imageWidget = _imageWidget;
	params = _params;
	fractalParams = _fractal;

	if (mainInterface->mainWindow)
	{
		ui = mainInterface->mainWindow->GetWidgetDockAnimation()->GetUi();

		// connect keyframe control buttons
		QApplication::connect(
			ui->pushButton_add_keyframe, SIGNAL(clicked()), this, SLOT(slotAddKeyframe()));
		QApplication::connect(
			ui->pushButton_insert_keyframe, SIGNAL(clicked()), this, SLOT(slotInsertKeyframe()));
		QApplication::connect(
			ui->pushButton_delete_keyframe, SIGNAL(clicked()), this, SLOT(slotDeleteKeyframe()));
		QApplication::connect(
			ui->pushButton_modify_keyframe, SIGNAL(clicked()), this, SLOT(slotModifyKeyframe()));
		QApplication::connect(ui->pushButton_render_keyframe_animation, SIGNAL(clicked()), this,
			SLOT(slotRenderKeyframes()));
		QApplication::connect(ui->pushButton_delete_all_keyframe_images, SIGNAL(clicked()), this,
			SLOT(slotDeleteAllImages()));
		QApplication::connect(
			ui->pushButton_show_keyframe_animation, SIGNAL(clicked()), this, SLOT(slotShowAnimation()));
		QApplication::connect(
			ui->pushButton_refresh_keyframe_table, SIGNAL(clicked()), this, SLOT(slotRefreshTable()));
		QApplication::connect(ui->pushButton_keyframe_to_flight_export, SIGNAL(clicked()), this,
			SLOT(slotExportKeyframesToFlight()));
		QApplication::connect(
			ui->pushButton_check_for_collisions, SIGNAL(clicked()), this, SLOT(slotValidate()));
		QApplication::connect(ui->pushButton_set_constant_target_distance, SIGNAL(clicked()), this,
			SLOT(slotSetConstantTargetDistance()));
		QApplication::connect(ui->button_selectAnimKeyframeImageDir, SIGNAL(clicked()), this,
			SLOT(slotSelectKeyframeAnimImageDir()));
		QApplication::connect(ui->tableWidget_keyframe_animation, SIGNAL(cellChanged(int, int)), this,
			SLOT(slotTableCellChanged(int, int)));
		QApplication::connect(ui->spinboxInt_keyframe_first_to_render, SIGNAL(valueChanged(int)), this,
			SLOT(slotMovedSliderFirstFrame(int)));
		QApplication::connect(ui->spinboxInt_keyframe_last_to_render, SIGNAL(valueChanged(int)), this,
			SLOT(slotMovedSliderLastFrame(int)));
		QApplication::connect(ui->spinboxInt_frames_per_keyframe, SIGNAL(valueChanged(int)), this,
			SLOT(UpdateLimitsForFrameRange()));
		QApplication::connect(ui->tableWidget_keyframe_animation, SIGNAL(cellDoubleClicked(int, int)),
			this, SLOT(slotCellDoubleClicked(int, int)));

		// connect system tray
		connect(mainInterface->systemTray, SIGNAL(notifyRenderKeyframes()), this,
			SLOT(slotRenderKeyframes()));
		connect(this, SIGNAL(notifyRenderKeyframeRenderStatus(QString, QString)),
			mainInterface->systemTray, SLOT(showMessage(QString, QString)));

		QApplication::connect(this, SIGNAL(QuestionMessage(const QString, const QString,
																	QMessageBox::StandardButtons, QMessageBox::StandardButton *)),
			mainInterface->mainWindow, SLOT(slotQuestionMessage(const QString, const QString,
																	 QMessageBox::StandardButtons, QMessageBox::StandardButton *)));

		table = ui->tableWidget_keyframe_animation;

		// add default parameters for animation
		if (keyframes->GetListOfUsedParameters().size() == 0)
		{
			keyframes->AddAnimatedParameter("camera", params->GetAsOneParameter("camera"), params);
			keyframes->AddAnimatedParameter("target", params->GetAsOneParameter("target"), params);
			keyframes->AddAnimatedParameter(
				"camera_top", params->GetAsOneParameter("camera_top"), params);
			if (mainInterface->mainWindow) PrepareTable();
		}
	}
	else
	{
		ui = nullptr;
		table = nullptr;
	}

	QApplication::connect(this,
		SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)), gErrorMessage,
		SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));
}

void cKeyframeAnimation::slotAddKeyframe()
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo.Store(params, fractalParams, nullptr, keyframes);

	NewKeyframe(keyframes->GetNumberOfFrames());
}

void cKeyframeAnimation::slotInsertKeyframe()
{
	int index = table->currentColumn() - reservedColums;
	if (index < 0) index = 0;

	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo.Store(params, fractalParams, nullptr, keyframes);

	NewKeyframe(index);
}

void cKeyframeAnimation::NewKeyframe(int index)
{
	if (keyframes)
	{
		// add new frame to container
		keyframes->AddFrame(*params, *fractalParams, index);

		params->Set("frame_no", keyframes->GetFramesPerKeyframe() * index);

		// add column to table
		int newColumn = AddColumn(keyframes->GetFrame(index), index);
		table->selectColumn(newColumn);

		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, 1, table);
			thumbWidget->UseOneCPUCore(false);
			thumbWidget->AssignParameters(*params, *fractalParams);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		UpdateLimitsForFrameRange();
		ui->spinboxInt_keyframe_last_to_render->setValue(
			keyframes->GetNumberOfFrames() * ui->spinboxInt_frames_per_keyframe->value());
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cKeyframeAnimation::DeleteKeyframe(int index) const
{
	if (index < 0)
	{
		cErrorMessage::showMessage(QObject::tr("No keyframe selected"), cErrorMessage::errorMessage,
			mainInterface->mainWindow->GetCentralWidget());
	}
	else
	{
		gUndo.Store(params, fractalParams, nullptr, keyframes);
		keyframes->DeleteFrames(index, index);
		table->removeColumn(index + reservedColums);
		UpdateLimitsForFrameRange();
	}
}

void cKeyframeAnimation::slotModifyKeyframe()
{
	int index = table->currentColumn() - reservedColums;
	if (index < 0)
	{
		cErrorMessage::showMessage(QObject::tr("No keyframe selected"), cErrorMessage::errorMessage,
			mainInterface->mainWindow->GetCentralWidget());
	}
	else
	{
		if (keyframes)
		{
			// get latest values of all parameters
			mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
			gUndo.Store(params, fractalParams, nullptr, keyframes);

			// add new frame to container
			keyframes->DeleteFrames(index, index);
			keyframes->AddFrame(*params, *fractalParams, index);

			// add column to table
			table->removeColumn(index + reservedColums);
			int newColumn = AddColumn(keyframes->GetFrame(index), index);
			table->selectColumn(newColumn);

			if (ui->checkBox_show_keyframe_thumbnails->isChecked())
			{
				cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, 1, table);
				thumbWidget->UseOneCPUCore(false);
				thumbWidget->AssignParameters(*params, *fractalParams);
				table->setCellWidget(0, newColumn, thumbWidget);
			}
		}
		else
		{
			qCritical() << "gAnimFrames not allocated";
		}
	}
}

void cKeyframeAnimation::slotDeleteKeyframe() const
{
	int index = table->currentColumn() - reservedColums;
	DeleteKeyframe(index);
}

bool cKeyframeAnimation::slotRenderKeyframes()
{
	// get latest values of all parameters
	if (mainInterface->mainWindow)
	{
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	}

	if (keyframes)
	{
		if (keyframes->GetNumberOfFrames() == 0)
		{
			emit showErrorMessage(QObject::tr("No frames to render"), cErrorMessage::errorMessage,
				mainInterface->mainWindow->GetCentralWidget());
		}
		else if (!QDir(params->Get<QString>("anim_keyframe_dir")).exists())
		{
			emit showErrorMessage(
				QObject::tr("The folder %1 does not exist. Please specify a valid location.")
					.arg(params->Get<QString>("anim_keyframe_dir")),
				cErrorMessage::errorMessage, mainInterface->mainWindow->GetCentralWidget());
		}
		else
		{
			return RenderKeyframes(&gMainInterface->stopRequest);
		}
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
	return false;
}

void cKeyframeAnimation::PrepareTable()
{
	table->setRowCount(0);
	table->setColumnCount(0);
	table->clear();
	tableRowNames.clear();
	table->verticalHeader()->setDefaultSectionSize(params->Get<int>("ui_font_size") + 6);
	AddAnimSoundColumn();
	CreateRowsInTable();
}

void cKeyframeAnimation::CreateRowsInTable()
{
	QList<cAnimationFrames::sParameterDescription> parList = keyframes->GetListOfUsedParameters();
	table->setIconSize(QSize(100, 70));
	table->insertRow(0);
	table->setVerticalHeaderItem(0, new QTableWidgetItem(tr("Keyframe\npreviews")));
	table->setRowHeight(0, 70);
	tableRowNames.append(tr("Keyframe\npreviews"));

	rowParameter.clear();
	rowParameter.append(-1);

	parameterRows.clear();
	for (int i = 0; i < parList.size(); ++i)
	{
		int row = AddVariableToTable(parList[i], i);
		parameterRows.append(row);
	}
}

int cKeyframeAnimation::AddVariableToTable(
	const cAnimationFrames::sParameterDescription &parameterDescription, int index)
{
	using namespace parameterContainer;
	enumVarType type = parameterDescription.varType;
	int row = table->rowCount();
	if (type == typeVector3)
	{
		QString varName;
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
		AddRow(row, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_y";
		AddRow(row + 1, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_z";
		AddRow(row + 2, varName, index);
	}
	else if (type == typeVector4)
	{
		QString varName;
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
		AddRow(row, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_y";
		AddRow(row + 1, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_z";
		AddRow(row + 2, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_w";
		AddRow(row + 3, varName, index);
	}
	else if (type == typeRgb)
	{
		QString varName;
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_R";
		AddRow(row, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_G";
		AddRow(row + 1, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_B";
		AddRow(row + 2, varName, index);
	}
	else
	{
		QString varName = parameterDescription.containerName + "_" + parameterDescription.parameterName;
		AddRow(row, varName, index);
	}
	return row;
}

void cKeyframeAnimation::AddRow(int row, const QString &fullParameterName, int index)
{
	tableRowNames.append(fullParameterName);
	table->insertRow(row);
	table->setVerticalHeaderItem(row, new QTableWidgetItem(fullParameterName));
	rowParameter.append(index);
	table->setCellWidget(row, animSoundColum, new cPushButtonAnimSound(table));
	static_cast<cPushButtonAnimSound *>(table->cellWidget(row, animSoundColum))
		->AssignParameterName(fullParameterName);
	static_cast<cPushButtonAnimSound *>(table->cellWidget(row, animSoundColum))
		->AssignAnimation(keyframes);
}

int cKeyframeAnimation::AddColumn(const cAnimationFrames::sAnimationFrame &frame, int index)
{
	table->blockSignals(true);
	int newColumn = index + reservedColums;
	if (index == -1) newColumn = table->columnCount();
	table->insertColumn(newColumn);

	double time = params->Get<double>("frames_per_keyframe")
								/ params->Get<double>("keyframe_frames_per_second") * (newColumn - reservedColums);
	int minutes = int(time / 60);
	int seconds = int(time) % 60;
	QString columnHeader = QString("%1 (%2:%3)")
													 .arg(newColumn - reservedColums)
													 .arg(minutes)
													 .arg(seconds, 2, 10, QChar('0'));
	table->setHorizontalHeaderItem(newColumn, new QTableWidgetItem(columnHeader));

	QList<cAnimationFrames::sParameterDescription> parList = keyframes->GetListOfUsedParameters();

	using namespace parameterContainer;
	for (int i = 0; i < parList.size(); ++i)
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
			table->setItem(
				row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 16)));
			table->setItem(
				row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 16)));
			table->item(row, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 1, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 2, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row, newColumn)->setTextColor(Qt::black);
			table->item(row + 1, newColumn)->setTextColor(Qt::black);
			table->item(row + 2, newColumn)->setTextColor(Qt::black);
		}
		else if (type == typeVector4)
		{
			CVector4 val = parameter.Get<CVector4>(valueActual);
			table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 16)));
			table->setItem(
				row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 16)));
			table->setItem(
				row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 16)));
			table->setItem(
				row + 3, newColumn, new QTableWidgetItem(QString("%L1").arg(val.w, 0, 'g', 16)));
			table->item(row, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 1, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 2, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row + 3, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row, newColumn)->setTextColor(Qt::black);
			table->item(row + 1, newColumn)->setTextColor(Qt::black);
			table->item(row + 2, newColumn)->setTextColor(Qt::black);
			table->item(row + 3, newColumn)->setTextColor(Qt::black);
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
			table->item(row, newColumn)->setTextColor(Qt::black);
			table->item(row + 1, newColumn)->setTextColor(Qt::black);
			table->item(row + 2, newColumn)->setTextColor(Qt::black);
		}
		else
		{
			QString val = parameter.Get<QString>(valueActual);
			table->setItem(row, newColumn, new QTableWidgetItem(val));
			table->item(row, newColumn)->setBackgroundColor(MorphType2Color(morphType));
			table->item(row, newColumn)->setTextColor(Qt::black);
		}
	}
	table->blockSignals(false);
	return newColumn;
}

QColor cKeyframeAnimation::MorphType2Color(parameterContainer::enumMorphType morphType)
{
	using namespace parameterContainer;
	QColor color;
	switch (morphType)
	{
		case morphNone: color = QColor(255, 255, 255); break;
		case morphLinear: color = QColor(200, 200, 200); break;
		case morphLinearAngle: color = QColor(200, 200, 255); break;
		case morphCatMullRom: color = QColor(200, 255, 200); break;
		case morphCatMullRomAngle: color = QColor(200, 255, 255); break;
		case morphAkima: color = QColor(255, 200, 200); break;
		case morphAkimaAngle: color = QColor(255, 200, 255); break;
	}
	return color;
}

bool cKeyframeAnimation::RenderKeyframes(bool *stopRequest)
{
	mainInterface->DisablePeriodicRefresh();

	if (image->IsUsed())
	{
		emit showErrorMessage(
			QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"),
			cErrorMessage::errorMessage);
		return false;
	}

	*stopRequest = false;

	// preparing Render Job
	cRenderJob *renderJob = new cRenderJob(params, fractalParams, image, stopRequest, imageWidget);
	connect(renderJob, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(
		renderJob, SIGNAL(updateStatistics(cStatistics)), this, SIGNAL(updateStatistics(cStatistics)));

	cRenderingConfiguration config;
	config.EnableNetRender();

	if (systemData.noGui)
	{
		config.DisableRefresh();
		config.DisableProgressiveRender();
		config.EnableNetRender();
	}

	renderJob->Init(cRenderJob::keyframeAnim, config);

	cProgressText progressText;
	progressText.ResetTimer();

	// range of keyframes to render
	int startFrame = params->Get<int>("keyframe_first_to_render");
	int endFrame = params->Get<int>("keyframe_last_to_render");

	int frames_per_keyframe = params->Get<int>("frames_per_keyframe");

	if (endFrame == 0) endFrame = keyframes->GetNumberOfFrames() * frames_per_keyframe;

	if (startFrame == endFrame)
	{
		emit showErrorMessage(
			QObject::tr(
				"There is no frame to render: first frame to render and last frame to render are equals."),
			cErrorMessage::warningMessage);
		delete renderJob;
		return false;
	}

	try
	{
		// updating parameters
		if (!systemData.noGui && image->IsMainImage())
		{
			mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
			gUndo.Store(params, fractalParams, nullptr, keyframes);
		}

		keyframes->SetFramesPerKeyframe(frames_per_keyframe);

		keyframes->RefreshAllAudioTracks(params);

		// checking for collisions
		if (!systemData.noGui && image->IsMainImage())
		{
			if (params->Get<bool>("keyframe_auto_validate"))
			{
				gKeyframes->ClearMorphCache();

				QList<int> listOfCollisions =
					CheckForCollisions(params->Get<double>("keyframe_collision_thresh"), stopRequest);
				if (*stopRequest) throw false;
				if (listOfCollisions.size() > 0)
				{
					QString collisionText;
					for (int i = 0; i < listOfCollisions.size(); i++)
					{
						collisionText += QString("%1").arg(listOfCollisions.at(i));
						if (i < listOfCollisions.size() - 1) collisionText += QString(", ");
					}
					emit showErrorMessage(
						QObject::tr("Camera collides with fractal at following frames:\n") + collisionText,
						cErrorMessage::warningMessage);
				}
			}
		}

		// Check if frames have already been rendered
		for (int index = 0; index < keyframes->GetNumberOfFrames() - 1; ++index)
		{
			cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);
			frame.alreadyRenderedSubFrames.clear();
			for (int subIndex = 0; subIndex < keyframes->GetFramesPerKeyframe(); subIndex++)
			{
				QString filename = GetKeyframeFilename(index, subIndex);
				int frameNo = index * keyframes->GetFramesPerKeyframe() + subIndex;
				frame.alreadyRenderedSubFrames.append(
					QFile(filename).exists() || frameNo < startFrame || frameNo >= endFrame);
			}
			keyframes->ModifyFrame(index, frame);
		}
		int unrenderedTotal = keyframes->GetUnrenderedTotal();

		// message if all frames are already rendered
		if (keyframes->GetNumberOfFrames() - 1 > 0 && unrenderedTotal == 0)
		{
			bool deletePreviousRender;
			QString questionTitle = QObject::tr("Truncate Image Folder");
			QString questionText =
				QObject::tr(
					"The animation has already been rendered completely.\n Do you want to purge the output "
					"folder?\n")
				+ QObject::tr("This will delete all images in the image folder.\nProceed?");

			if (!systemData.noGui)
			{
				QMessageBox::StandardButton reply = QMessageBox::NoButton;
				emit QuestionMessage(
					questionTitle, questionText, QMessageBox::Yes | QMessageBox::No, &reply);
				while (reply == QMessageBox::NoButton)
				{
					gApplication->processEvents();
				}
				deletePreviousRender = (reply == QMessageBox::Yes);
			}
			else
			{
				// Exit if silent mode
				if (systemData.silent)
				{
					exit(0);
				}

				deletePreviousRender = cHeadless::ConfirmMessage(questionTitle + "\n" + questionText);
			}

			if (deletePreviousRender)
			{
				DeleteAllFilesFromDirectory(params->Get<QString>("anim_keyframe_dir"), "frame_?????.*");
				delete renderJob;
				return RenderKeyframes(stopRequest);
			}
			else
			{
				throw false;
			}
		}

		// total number of frames
		int totalFrames = (keyframes->GetNumberOfFrames() - 1) * keyframes->GetFramesPerKeyframe();

		gKeyframes->ClearMorphCache();

		// main loop for rendering of frames
		for (int index = 0; index < keyframes->GetNumberOfFrames() - 1; ++index)
		{
			//-------------- rendering of interpolated keyframes ----------------
			for (int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
			{
				// skip already rendered frame
				if (keyframes->GetFrame(index).alreadyRenderedSubFrames[subindex])
				{
					continue;
				}

				int frameIndex = index * keyframes->GetFramesPerKeyframe() + subindex;

				double percentDoneFrame;
				if (unrenderedTotal > 0)
					percentDoneFrame =
						(keyframes->GetUnrenderedTillIndex(frameIndex) * 1.0) / unrenderedTotal;
				else
					percentDoneFrame = 1.0;

				QString progressTxt = progressText.getText(percentDoneFrame);

				emit updateProgressAndStatus(QObject::tr("Rendering animation"),
					QObject::tr("Frame %1 of %2").arg((frameIndex + 1)).arg(totalFrames) + " " + progressTxt,
					percentDoneFrame, cProgressText::progress_ANIMATION);

				if (*stopRequest) throw false;
				keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, params, fractalParams);

				// recalculation of camera rotation and distance (just for display purposes)
				CVector3 camera = params->Get<CVector3>("camera");
				CVector3 target = params->Get<CVector3>("target");
				CVector3 top = params->Get<CVector3>("camera_top");
				cCameraTarget cameraTarget(camera, target, top);
				params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
				params->Set("camera_distance_to_target", cameraTarget.GetDistance());

				if (!systemData.noGui && image->IsMainImage())
				{
					mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

					// show distance in statistics table
					double distance = mainInterface->GetDistanceForPoint(
						params->Get<CVector3>("camera"), params, fractalParams);
					mainInterface->mainWindow->GetWidgetDockStatistics()->UpdateDistanceToFractal(distance);
				}

				if (gNetRender->IsServer())
				{
					gNetRender->WaitForAllClientsReady(10.0);
				}

				params->Set("frame_no", frameIndex);
				renderJob->UpdateParameters(params, fractalParams);
				int result = renderJob->Execute();
				if (!result) throw false;
				QString filename = GetKeyframeFilename(index, subindex);
				ImageFileSave::enumImageFileType fileType =
					ImageFileSave::enumImageFileType(params->Get<int>("keyframe_animation_image_type"));
				SaveImage(filename, fileType, image, gMainInterface->mainWindow);
			}
			//--------------------------------------------------------------------
		}

		emit updateProgressAndStatus(QObject::tr("Animation finished"), progressText.getText(1.0), 1.0,
			cProgressText::progress_IMAGE);
		emit updateProgressHide();
		emit notifyRenderKeyframeRenderStatus(
			QObject::tr("Animation finished"), progressText.getText(1.0));
	}
	catch (bool ex)
	{
		QString resultStatus = QObject::tr("Rendering terminated");
		if (ex) resultStatus += " - " + QObject::tr("Error occured, see log output");
		emit updateProgressAndStatus(
			resultStatus, progressText.getText(1.0), cProgressText::progress_ANIMATION);
		emit updateProgressHide();
		delete renderJob;
		return false;
	}

	delete renderJob;
	return true;
}

void cKeyframeAnimation::RefreshTable()
{
	mainInterface->progressBarAnimation->show();
	PrepareTable();
	gApplication->processEvents();

	int noOfFrames = keyframes->GetNumberOfFrames();

	UpdateLimitsForFrameRange(); // it is needed to do it also here, because limits must be set just
															 // after loading of settings
	SynchronizeInterfaceWindow(ui->tab_keyframe_animation, params, qInterface::read);

	cParameterContainer tempPar = *params;
	cFractalContainer tempFract = *fractalParams;

	for (int i = 0; i < noOfFrames; i++)
	{
		int newColumn = AddColumn(keyframes->GetFrame(i));

		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, 1, table);
			thumbWidget->UseOneCPUCore(true);
			keyframes->GetFrameAndConsolidate(i, &tempPar, &tempFract);
			tempPar.Set("frame_no", keyframes->GetFramesPerKeyframe() * i);
			thumbWidget->AssignParameters(tempPar, tempFract);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		if (i % 100 == 0)
		{
			emit updateProgressAndStatus(QObject::tr("Refreshing animation"),
				tr("Refreshing animation frames"), double(i) / noOfFrames,
				cProgressText::progress_ANIMATION);
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
	if (parameterNumber >= 0)
	{
		fullParameterName =
			list[parameterNumber].containerName + "_" + list[parameterNumber].parameterName;
	}
	else
	{
		qCritical() << "cFlightAnimation::GetParameterNumber(int rowNumber): row not found";
	}
	return fullParameterName;
}

void cKeyframeAnimation::RenderFrame(int index) const
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	keyframes->GetFrameAndConsolidate(index, params, fractalParams);

	// recalculation of camera rotation and distance (just for display purposes)
	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 top = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, top);
	params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
	params->Set("camera_distance_to_target", cameraTarget.GetDistance());

	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

	params->Set("frame_no", keyframes->GetFramesPerKeyframe() * index);

	mainInterface->StartRender();
}

void cKeyframeAnimation::DeleteFramesFrom(int index) const
{
	gUndo.Store(params, fractalParams, nullptr, keyframes);
	for (int i = keyframes->GetNumberOfFrames() - 1; i >= index; i--)
		table->removeColumn(index + reservedColums);
	keyframes->DeleteFrames(index, keyframes->GetNumberOfFrames() - 1);
	UpdateLimitsForFrameRange();
}

void cKeyframeAnimation::DeleteFramesTo(int index) const
{
	gUndo.Store(params, fractalParams, nullptr, keyframes);
	for (int i = 0; i <= index; i++)
		table->removeColumn(reservedColums);
	keyframes->DeleteFrames(0, index);
	UpdateLimitsForFrameRange();
}

void cKeyframeAnimation::slotSelectKeyframeAnimImageDir() const
{
	QFileDialog *dialog = new QFileDialog();
	dialog->setFileMode(QFileDialog::DirectoryOnly);
	dialog->setNameFilter(QObject::tr("Animation Image Folder"));
	dialog->setDirectory(QDir::toNativeSeparators(params->Get<QString>("anim_keyframe_dir")));
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setWindowTitle(QObject::tr("Choose Animation Image Folder"));
	dialog->setOption(QFileDialog::ShowDirsOnly);
	QStringList filenames;

	if (dialog->exec())
	{
		filenames = dialog->selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first() + QDir::separator());
		ui->text_anim_keyframe_dir->setText(filename);
		params->Set("anim_keyframe_dir", filename);
	}
}

void cKeyframeAnimation::slotTableCellChanged(int row, int column)
{
	if (row > 0)
	{
		table->blockSignals(true);
		QTableWidgetItem *cell = table->item(row, column);
		QString cellText = cell->text();

		int index = column - reservedColums;
		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);

		QString parameterName = GetParameterName(row);
		int parameterFirstRow = parameterRows[rowParameter[row]];
		int vectIndex = row - parameterFirstRow;

		using namespace parameterContainer;
		enumVarType type = frame.parameters.GetVarType(parameterName);

		if (type == typeVector3)
		{
			CVector3 vect = frame.parameters.Get<CVector3>(parameterName);
			if (vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if (vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if (vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if (type == typeVector4)
		{
			CVector4 vect = frame.parameters.Get<CVector4>(parameterName);
			if (vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if (vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if (vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			if (vectIndex == 3) vect.w = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if (type == typeRgb)
		{
			sRGB col = frame.parameters.Get<sRGB>(parameterName);
			if (vectIndex == 0) col.R = cellText.toInt();
			if (vectIndex == 1) col.G = cellText.toInt();
			if (vectIndex == 2) col.B = cellText.toInt();
			frame.parameters.Set(parameterName, col);
		}
		else
		{
			frame.parameters.Set(parameterName, cellText);
		}

		keyframes->ModifyFrame(index, frame);

		// update thumbnail
		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			cParameterContainer tempPar = *params;
			cFractalContainer tempFract = *fractalParams;
			keyframes->GetFrameAndConsolidate(index, &tempPar, &tempFract);
			cThumbnailWidget *thumbWidget = static_cast<cThumbnailWidget *>(table->cellWidget(0, column));

			if (!thumbWidget)
			{
				thumbWidget = new cThumbnailWidget(100, 70, 1, table);
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

void cKeyframeAnimation::slotDeleteAllImages() const
{
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_keyframeAnimationParameters, params, qInterface::read);

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(mainInterface->mainWindow->GetCentralWidget(),
		QObject::tr("Truncate Image Folder"),
		QObject::tr("This will delete all images in the image folder.\nProceed?"),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		DeleteAllFilesFromDirectory(params->Get<QString>("anim_keyframe_dir"), "frame_?????.*");
	}
}

void cKeyframeAnimation::slotShowAnimation() const
{
	WriteLog("Prepare PlayerWidget class", 2);

	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_keyframeAnimationParameters, params, qInterface::read);

	if (!mainInterface->imageSequencePlayer)
	{
		mainInterface->imageSequencePlayer = new PlayerWidget;
	}

	mainInterface->imageSequencePlayer->SetFilePath(params->Get<QString>("anim_keyframe_dir"));
	mainInterface->imageSequencePlayer->show();
	mainInterface->imageSequencePlayer->raise();
	mainInterface->imageSequencePlayer->activateWindow();
}

void cKeyframeAnimation::InterpolateForward(int row, int column)
{
	gUndo.Store(params, fractalParams, nullptr, keyframes);

	QTableWidgetItem *cell = table->item(row, column);
	QString cellText = cell->text();

	QString parameterName = GetParameterName(row);

	int index = column - reservedColums;
	cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);

	using namespace parameterContainer;
	enumVarType type = frame.parameters.GetVarType(parameterName);

	bool valueIsInteger = false;
	bool valueIsDouble = false;
	bool valueIsText = false;
	int valueInteger = 0;
	double valueDouble = 0.0;
	QString valueText;

	bool ok;
	int lastFrame = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation",
		"Enter last keyframe number", index, index + 1, keyframes->GetNumberOfFrames() - 1, 1, &ok);
	if (!ok) return;

	int numberOfFrames = (lastFrame - index);

	switch (type)
	{
		case typeBool:
		case typeInt:
		case typeRgb:
		{
			valueIsInteger = true;
			valueInteger = cellText.toInt();
			// qDebug() << valueInteger;
			break;
		}
		case typeDouble:
		case typeVector3:
		case typeVector4:
		{
			valueIsDouble = true;
			valueDouble = systemData.locale.toDouble(cellText);
			// qDebug() << valueDouble;
			break;
		}
		default:
		{
			valueIsText = true;
			valueText = cellText;
			break;
		}
	}

	double finalDouble;
	int finalInteger;
	double integerStep = 0.0;
	double doubleStep = 0.0;

	if (valueIsInteger)
	{
		finalInteger = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation",
			"Enter value for last keyframe", valueInteger, 0, 2147483647, 1, &ok);
		integerStep = double(finalInteger - valueInteger) / numberOfFrames;
	}
	else if (valueIsDouble)
	{
		finalDouble = systemData.locale.toDouble(QInputDialog::getText(mainInterface->mainWindow,
			"Parameter interpolation", "Enter value for last keyframe", QLineEdit::Normal,
			QString("%L1").arg(valueDouble, 0, 'g', 16), &ok));
		doubleStep = (finalDouble - valueDouble) / numberOfFrames;
	}

	if (!ok) return;

	for (int i = index + 1; i <= lastFrame; i++)
	{
		QString newCellText;
		if (valueIsInteger)
		{
			int newValue = int(integerStep * (i - index) + valueInteger);
			newCellText = QString::number(newValue);
		}
		else if (valueIsDouble)
		{
			double newValue = doubleStep * (i - index) + valueDouble;
			newCellText = QString("%L1").arg(newValue, 0, 'g', 16);
		}
		else if (valueIsText)
		{
			newCellText = valueText;
		}
		QTableWidgetItem *newCell = table->item(row, i + reservedColums);
		newCell->setText(newCellText);
		newCell->setTextColor(Qt::black);
	}
}

void cKeyframeAnimation::slotRefreshTable()
{
	RefreshTable();
}

QString cKeyframeAnimation::GetKeyframeFilename(int index, int subIndex) const
{
	int frameIndex = index * keyframes->GetFramesPerKeyframe() + subIndex;
	QString filename = params->Get<QString>("anim_keyframe_dir") + "frame_"
										 + QString("%1").arg(frameIndex, 5, 10, QChar('0'));
	filename += "." + ImageFileSave::ImageFileExtension(ImageFileSave::enumImageFileType(
											params->Get<int>("keyframe_animation_image_type")));
	return filename;
}

parameterContainer::enumMorphType cKeyframeAnimation::GetMorphType(int row) const
{
	parameterContainer::enumMorphType morphType;
	int parameterIndex = rowParameter.at(row);
	morphType = keyframes->GetListOfParameters().at(parameterIndex).morphType;
	return morphType;
}

void cKeyframeAnimation::ChangeMorphType(int row, parameterContainer::enumMorphType morphType)
{
	gUndo.Store(params, fractalParams, nullptr, keyframes);
	int parameterIndex = rowParameter.at(row);
	keyframes->ChangeMorphType(parameterIndex, morphType);
	RefreshTable();
}

void cKeyframeAnimation::slotExportKeyframesToFlight()
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo.Store(params, fractalParams, gAnimFrames, keyframes);
	keyframes->SetFramesPerKeyframe(params->Get<int>("frames_per_keyframe"));

	if (gAnimFrames->GetFrames().size() > 0)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Export keyframes to flight"),
			QObject::tr(
				"There are already captured flight frames present.\nDiscard current flight frames ?"),
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::No) return;
	}

	gAnimFrames->ClearAll();
	gAnimFrames->SetListOfParametersAndClear(gKeyframes->GetListOfParameters(), params);

	for (int index = 0; index < keyframes->GetNumberOfFrames() - 1; ++index)
	{
		for (int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
		{
			int frameIndex = index * keyframes->GetFramesPerKeyframe() + subindex;
			gAnimFrames->AddFrame(keyframes->GetInterpolatedFrame(frameIndex));
		}
		if (index % 10 == 0)
		{
			updateProgressAndStatus(QObject::tr("Exporting"), tr("Exporting keyframes to flight"),
				double(index) / keyframes->GetNumberOfFrames(), cProgressText::progress_ANIMATION);
			gApplication->processEvents();
		}
	}
	mainInterface->progressBarAnimation->hide();
	ui->tabWidgetFlightKeyframe->setCurrentIndex(0);
	ui->pushButton_flight_refresh_table->animateClick();
}

void cKeyframeAnimation::UpdateLimitsForFrameRange() const
{
	int framesPerKey = ui->spinboxInt_frames_per_keyframe->value();

	int noOfFrames = (keyframes->GetNumberOfFrames() - 1) * framesPerKey;
	if (noOfFrames < 0) noOfFrames = 0;

	ui->spinboxInt_keyframe_first_to_render->setMaximum(noOfFrames);
	ui->sliderInt_keyframe_first_to_render->setMaximum(noOfFrames);

	ui->spinboxInt_keyframe_last_to_render->setMaximum(noOfFrames);
	ui->sliderInt_keyframe_last_to_render->setMaximum(noOfFrames);

	ui->spinboxInt_keyframe_first_to_render->setValue(params->Get<int>("keyframe_first_to_render"));
	ui->spinboxInt_keyframe_last_to_render->setValue(params->Get<int>("keyframe_last_to_render"));
}

void cKeyframeAnimation::slotMovedSliderFirstFrame(int value) const
{
	if (value > ui->spinboxInt_keyframe_last_to_render->value())
		ui->spinboxInt_keyframe_last_to_render->setValue(value);
}
void cKeyframeAnimation::slotMovedSliderLastFrame(int value) const
{
	if (value < ui->spinboxInt_keyframe_first_to_render->value())
		ui->spinboxInt_keyframe_first_to_render->setValue(value);
}

QList<int> cKeyframeAnimation::CheckForCollisions(double minDist, bool *stopRequest)
{
	QList<int> listOfCollisions;
	cParameterContainer tempPar = *params;
	cFractalContainer tempFractPar = *fractalParams;

	*stopRequest = false;

	for (int key = 0; key < keyframes->GetNumberOfFrames() - 1; key++)
	{
		updateProgressAndStatus(QObject::tr("Checking for collissions"),
			QObject::tr("Checking for collissions on keyframe # %1").arg(key),
			double(key) / (keyframes->GetNumberOfFrames() - 1.0), cProgressText::progress_ANIMATION);

		for (int subindex = 0; subindex < keyframes->GetFramesPerKeyframe(); subindex++)
		{
			gApplication->processEvents();
			if (*stopRequest) return listOfCollisions;
			int frameIndex = key * keyframes->GetFramesPerKeyframe() + subindex;
			keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, &tempPar, &tempFractPar);
			tempPar.Set("frame_no", frameIndex);
			CVector3 point = tempPar.Get<CVector3>("camera");
			double dist = mainInterface->GetDistanceForPoint(point, &tempPar, &tempFractPar);
			if (dist < minDist)
			{
				listOfCollisions.append(frameIndex);
			}
		}
	}

	updateProgressAndStatus(QObject::tr("Checking for collissions"),
		QObject::tr("Checking for collisions finished"), 1.0, cProgressText::progress_ANIMATION);

	return listOfCollisions;
}

void cKeyframeAnimation::slotValidate()
{
	// updating parameters
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo.Store(params, fractalParams, nullptr, keyframes);

	keyframes->SetFramesPerKeyframe(params->Get<int>("frames_per_keyframe"));

	// checking for collisions
	QList<int> listOfCollisions = CheckForCollisions(
		params->Get<double>("keyframe_collision_thresh"), &gMainInterface->stopRequest);
	if (listOfCollisions.size() > 0)
	{
		QString collisionText;
		for (int i = 0; i < listOfCollisions.size(); i++)
		{
			collisionText += QString("%1").arg(listOfCollisions.at(i));
			if (i < listOfCollisions.size() - 1) collisionText += QString(", ");
		}
		emit showErrorMessage(
			QObject::tr("Camera collides with fractal at following frames:\n") + collisionText,
			cErrorMessage::warningMessage);
	}
	else
	{
		emit showErrorMessage(QObject::tr("No collisions detected\n"), cErrorMessage::infoMessage);
	}
}

void cKeyframeAnimation::slotCellDoubleClicked(int row, int column) const
{
	if (row == 0)
	{
		RenderFrame(column - reservedColums);
	}
}

void cKeyframeAnimation::slotSetConstantTargetDistance()
{
	// updating parameters
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo.Store(params, fractalParams, nullptr, keyframes);

	double constDist = params->Get<double>("keyframe_constant_target_distance");

	for (int key = 0; key < keyframes->GetNumberOfFrames() - 1; key++)
	{
		cAnimationFrames::sAnimationFrame keyframe = keyframes->GetFrame(key);

		if (keyframe.parameters.IfExists("main_camera") && keyframe.parameters.IfExists("main_target")
				&& keyframe.parameters.IfExists("main_camera_top"))
		{
			CVector3 camera = keyframe.parameters.Get<CVector3>("main_camera");
			CVector3 target = keyframe.parameters.Get<CVector3>("main_target");
			CVector3 top = keyframe.parameters.Get<CVector3>("main_camera_top");

			cCameraTarget cameraTarget(camera, target, top);
			CVector3 forwardVector = cameraTarget.GetForwardVector();

			CVector3 newTarget = camera + forwardVector * constDist;

			keyframe.parameters.Set("main_target", newTarget);
			if (keyframe.parameters.IfExists("camera_distance_to_target"))
			{
				keyframe.parameters.Set("camera_distance_to_target", constDist);
			}

			keyframes->ModifyFrame(key, keyframe);
		}
		else
		{
			cErrorMessage::showMessage(
				QObject::tr("Cannot change target distance. Missing camera parameters in keyframes"),
				cErrorMessage::errorMessage, mainInterface->mainWindow->GetCentralWidget());
			return;
		}
	}
	RefreshTable();
}

void cKeyframeAnimation::AddAnimSoundColumn() const
{
	int newColumn = table->columnCount();
	table->insertColumn(newColumn);
	table->setHorizontalHeaderItem(newColumn, new QTableWidgetItem(tr("Audio")));
}
