/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * The class can render interpolated sub-frames and validate their path (check
 * for collision). It exposes slots to manipulate the keyframes.
 */

#include "animation_keyframes.hpp"

#include <memory>

#include <QFontMetrics>

#include "ui_dock_animation.h"

#include "animation_path_data.hpp"
#include "cimage.hpp"
#include "common_math.h"
#include "files.h"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "light.h"
#include "netrender.hpp"
#include "render_job.hpp"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"
#include "rendering_configuration.hpp"
#include "scripts.h"
#include "settings.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "undo.h"
#include "write_log.hpp"

#include "qt/dock_animation.h"
#include "qt/dock_navigation.h"
#include "qt/dock_statistics.h"
#include "qt/my_progress_bar.h"
#include "qt/my_table_widget_keyframes.hpp"
#include "qt/player_widget.hpp"
#include "qt/pushbutton_anim_sound.h"
#include "qt/randomizer_dialog.h"
#include "qt/system_tray.hpp"
#include "qt/thumbnail_widget.h"

cKeyframeAnimation *gKeyframeAnimation = nullptr;

cKeyframeAnimation::cKeyframeAnimation(cInterface *_interface, std::shared_ptr<cKeyframes> _frames,
	std::shared_ptr<cImage> _image, QWidget *_imageWidget,
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractal,
	QObject *parent)
		: QObject(parent), mainInterface(_interface), keyframes(_frames)
{
	image = _image;
	imageWidget = dynamic_cast<RenderedImage *>(_imageWidget);
	params = _params;
	fractalParams = _fractal;

	if (mainInterface && mainInterface->mainWindow)
	{
		ui = mainInterface->mainWindow->GetWidgetDockAnimation()->GetUi();

		previewSize.setWidth(systemData.GetPreferredThumbnailSize() * 5 / 4);
		previewSize.setHeight(previewSize.width() * 3 / 4);

		// connect keyframe control buttons
		connect(ui->pushButton_add_keyframe, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotAddKeyframe);
		connect(ui->pushButton_insert_keyframe, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotInsertKeyframe);
		connect(ui->pushButton_delete_keyframe, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotDeleteKeyframe);
		connect(ui->pushButton_modify_keyframe, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotModifyKeyframe);
		connect(ui->pushButton_render_keyframe_animation, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotRenderKeyframes);
		connect(ui->pushButton_delete_all_keyframe_images, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotDeleteAllImages);
		connect(ui->pushButton_show_keyframe_animation, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotShowAnimation);
		connect(ui->pushButton_refresh_keyframe_table, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotRefreshTable);
		connect(ui->pushButton_keyframe_to_flight_export, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotExportKeyframesToFlight);
		connect(ui->pushButton_check_for_collisions, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotValidate);
		connect(ui->pushButton_set_constant_target_distance, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotSetConstantTargetDistance);
		connect(ui->button_selectAnimKeyframeImageDir, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotSelectKeyframeAnimImageDir);
		connect(
			ui->pushButton_randomize, &QPushButton::clicked, this, &cKeyframeAnimation::slotRandomize);
		connect(ui->pushButton_add_all_parameters, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotAddAllParameters);
		connect(ui->pushButton_set_frames_per_keyframe_to_all, &QPushButton::clicked, this,
			&cKeyframeAnimation::slotSetFramesPerKeyframeToAllKeyframes);

		connect(ui->tableWidget_keyframe_animation, &QTableWidget::cellChanged, this,
			&cKeyframeAnimation::slotTableCellChanged);
		connect(ui->spinboxInt_keyframe_first_to_render, &QSpinBox::editingFinished, this,
			&cKeyframeAnimation::slotMovedSliderFirstFrame);
		connect(ui->spinboxInt_keyframe_last_to_render, &QSpinBox::editingFinished, this,
			&cKeyframeAnimation::slotMovedSliderLastFrame);
		connect(ui->spinboxInt_frames_per_keyframe, &QSpinBox::editingFinished, this,
			&cKeyframeAnimation::UpdateLimitsForFrameRange);
		connect(ui->tableWidget_keyframe_animation, &QTableWidget::cellDoubleClicked, this,
			&cKeyframeAnimation::slotCellDoubleClicked);
		connect(ui->tableWidget_keyframe_animation, &QTableWidget::cellClicked, this,
			&cKeyframeAnimation::slotCellClicked);
		connect(ui->horizontalSlider_actualFrame, &QSlider::sliderMoved, this,
			&cKeyframeAnimation::slotSliderMovedActualFrame);
		connect(ui->horizontalSlider_actualFrame, &QSlider::sliderMoved, ui->widgetValueChart,
			&cAnimationValueChartWidget::slotSetCurrentFrame);
		connect(ui->toolButton_next_frame, &QToolButton::clicked, this,
			&cKeyframeAnimation::slotClickedNextFrame);
		connect(ui->toolButton_previousFrame, &QToolButton::clicked, this,
			&cKeyframeAnimation::slotClickedPrevFrame);

		// connect system tray
		connect(mainInterface->systemTray, &cSystemTray::notifyRenderKeyframes, this,
			&cKeyframeAnimation::slotRenderKeyframes);
		connect(this, &cKeyframeAnimation::notifyRenderKeyframeRenderStatus, mainInterface->systemTray,
			&cSystemTray::showMessage);

		connect(this, &cKeyframeAnimation::QuestionMessage, mainInterface->mainWindow,
			&RenderWindow::slotQuestionMessage);

		connect(ui->checkBox_show_camera_path, &QCheckBox::stateChanged, this,
			&cKeyframeAnimation::slotUpdateAnimationPathSelection);
		connect(ui->checkBox_show_target_path, &QCheckBox::stateChanged, this,
			&cKeyframeAnimation::slotUpdateAnimationPathSelection);
		connect(ui->checkBox_show_light_path_1, &QCheckBox::stateChanged, this,
			&cKeyframeAnimation::slotUpdateAnimationPathSelection);
		connect(ui->checkBox_show_light_path_2, &QCheckBox::stateChanged, this,
			&cKeyframeAnimation::slotUpdateAnimationPathSelection);
		connect(ui->checkBox_show_light_path_3, &QCheckBox::stateChanged, this,
			&cKeyframeAnimation::slotUpdateAnimationPathSelection);
		connect(ui->checkBox_show_light_path_4, &QCheckBox::stateChanged, this,
			&cKeyframeAnimation::slotUpdateAnimationPathSelection);

		connect(ui->toolButton_chartZoomIn, &QToolButton::clicked, ui->widgetValueChart,
			&cAnimationValueChartWidget::slotZoomIn);
		connect(ui->toolButton_chartZoomOut, &QToolButton::clicked, ui->widgetValueChart,
			&cAnimationValueChartWidget::slotZoomOut);

		connect(ui->widgetValueChart, &cAnimationValueChartWidget::signalUpdateKey, this,
			&cKeyframeAnimation::slotUpdateKeyByChart);
		connect(ui->widgetValueChart, &cAnimationValueChartWidget::signalClearKey, this,
			&cKeyframeAnimation::slotClearKeyframe);

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

		ui->widgetValueChart->setVisible(false);
	}
	else
	{
		ui = nullptr;
		table = nullptr;
	}

	// NetRender for animation
	// signals to NetRender
	connect(this, &cKeyframeAnimation::SendNetRenderSetup, gNetRender, &cNetRender::SendSetup);
	connect(this, &cKeyframeAnimation::NetRenderCurrentAnimation, gNetRender,
		&cNetRender::SetCurrentAnimation);
	connect(this, &cKeyframeAnimation::NetRenderConfirmRendered, gNetRender,
		&cNetRender::ConfirmRenderedFrame);
	connect(
		this, &cKeyframeAnimation::NetRenderAddFileToSender, gNetRender, &cNetRender::AddFileToSender);
	connect(
		this, &cKeyframeAnimation::NetRenderNotifyClientStatus, gNetRender, &cNetRender::NotifyStatus);

	// signals from NetRender
	connect(
		gNetRender, &cNetRender::FinishedFrame, this, &cKeyframeAnimation::slotNetRenderFinishedFrame);
	connect(this, &cKeyframeAnimation::NetRenderSendFramesToDoList, gNetRender,
		&cNetRender::SendFramesToDoList);
	connect(gNetRender, &cNetRender::UpdateFramesToDo, this,
		&cKeyframeAnimation::slotNetRenderUpdateFramesToDo);
	connect(
		this, &cKeyframeAnimation::NetRenderStopAllClients, gNetRender, &cNetRender::StopAllClients);
	connect(gNetRender, &cNetRender::animationStopRequest, this,
		&cKeyframeAnimation::slotAnimationStopRequest);

	connect(this, SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)),
		gErrorMessage, SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));
}

void cKeyframeAnimation::slotAddKeyframe()
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo->Store(params, fractalParams, nullptr, keyframes);

	NewKeyframe(keyframes->GetNumberOfFrames());

	keyframes->UpdateFramesIndexesTable();
}

void cKeyframeAnimation::slotInsertKeyframe()
{
	int index = table->currentColumn() - reservedColumns;
	if (index < 0) index = 0;

	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo->Store(params, fractalParams, nullptr, keyframes);

	NewKeyframe(index);
}

void cKeyframeAnimation::NewKeyframe(int index)
{
	if (keyframes)
	{
		// add new frame to container
		keyframes->AddFrame(params, fractalParams, params->Get<int>("frames_per_keyframe"), index);
		keyframes->UpdateFramesIndexesTable();

		params->Set("frame_no", keyframes->GetFrameIndexForKeyframe(index));

		// add column to table
		const int newColumn = AddColumn(keyframes->GetFrame(index), index);
		table->selectColumn(newColumn);

		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			double dpiScale = table->devicePixelRatioF();
			cThumbnailWidget *thumbWidget =
				new cThumbnailWidget(previewSize.width(), previewSize.height(), dpiScale, table);
			thumbWidget->UseOneCPUCore(false);
			thumbWidget->AssignParameters(params, fractalParams);
			table->setCellWidget(0, newColumn, thumbWidget);
		}

		UpdateLimitsForFrameRange();

		UpdateAnimationPathCameraAndLights();
		UpdateAnimationPathSingleParameter();
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
		gUndo->Store(params, fractalParams, nullptr, keyframes);
		keyframes->DeleteFrames(index, index);
		table->removeColumn(index + reservedColumns);
		keyframes->UpdateFramesIndexesTable();
		UpdateLimitsForFrameRange();
		UpdateAnimationPathCameraAndLights();
		UpdateAnimationPathSingleParameter();
	}
}

void cKeyframeAnimation::slotModifyKeyframe()
{
	const int index = table->currentColumn() - reservedColumns;
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
			gUndo->Store(params, fractalParams, nullptr, keyframes);

			int framesPerKeyframe = keyframes->GetFrame(index).numberOfSubFrames;

			// add new frame to container
			keyframes->DeleteFrames(index, index);
			keyframes->AddFrame(params, fractalParams, framesPerKeyframe, index);

			keyframes->UpdateFramesIndexesTable();

			// add column to table
			table->removeColumn(index + reservedColumns);
			const int newColumn = AddColumn(keyframes->GetFrame(index), index);
			table->selectColumn(newColumn);

			if (ui->checkBox_show_keyframe_thumbnails->isChecked())
			{
				double dpiScale = table->devicePixelRatioF();
				cThumbnailWidget *thumbWidget =
					new cThumbnailWidget(previewSize.width(), previewSize.height(), dpiScale, table);
				thumbWidget->UseOneCPUCore(false);
				thumbWidget->AssignParameters(params, fractalParams);
				table->setCellWidget(0, newColumn, thumbWidget);
			}

			UpdateAnimationPathCameraAndLights();
			UpdateAnimationPathSingleParameter();
		}
		else
		{
			qCritical() << "gAnimFrames not allocated";
		}
	}
}

void cKeyframeAnimation::slotDeleteKeyframe() const
{
	const int index = table->currentColumn() - reservedColumns;
	DeleteKeyframe(index);

	keyframes->UpdateFramesIndexesTable();
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

void cKeyframeAnimation::slotIncreaseCurrentTableIndex()
{
	int nextColumn = (table->currentColumn() + 1) % table->columnCount();
	table->setCurrentCell(table->currentRow(), nextColumn);
}

void cKeyframeAnimation::slotDecreaseCurrentTableIndex()
{
	int nextColumn = (table->columnCount() + table->currentColumn() - 1) % table->columnCount();
	table->setCurrentCell(table->currentRow(), nextColumn);
}

void cKeyframeAnimation::PrepareTable()
{
	table->setRowCount(0);
	table->setColumnCount(0);
	table->clear();
	tableRowNames.clear();
	QFontMetrics fm(mainInterface->mainWindow->font());
	table->verticalHeader()->setDefaultSectionSize(fm.height() + 6);
	AddAnimSoundColumn();
	CreateRowsInTable();
}

void cKeyframeAnimation::CreateRowsInTable()
{
	QList<cAnimationFrames::sParameterDescription> parList = keyframes->GetListOfUsedParameters();
	table->setIconSize(previewSize);
	table->insertRow(0);
	table->setVerticalHeaderItem(0, new QTableWidgetItem(tr("Keyframe\npreviews")));
	table->setRowHeight(0, previewSize.height());
	tableRowNames.append(tr("Keyframe\npreviews"));

	table->insertRow(framesPerKeyframeRow);
	table->setVerticalHeaderItem(framesPerKeyframeRow, new QTableWidgetItem(tr("Frames/keyframe")));
	tableRowNames.append(tr("Frames/keyframe"));

	table->insertRow(cameraSpeedRow);
	table->setVerticalHeaderItem(
		cameraSpeedRow, new QTableWidgetItem(tr("Camera speed [units/frame]")));
	tableRowNames.append(tr("Camera speed [units/frame]"));

	rowParameter.clear();
	rowParameter.append(-1); // prevuew
	rowParameter.append(-1); // frames per keyframe
	rowParameter.append(-1); // camera speed

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
	const enumVarType type = parameterDescription.varType;
	const int row = table->rowCount();
	if (type == typeVector3)
	{
		QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
		AddRow(row, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_y";
		AddRow(row + 1, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_z";
		AddRow(row + 2, varName, index);
	}
	else if (type == typeVector4)
	{
		QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
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
		QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName + "_R";
		AddRow(row, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_G";
		AddRow(row + 1, varName, index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_B";
		AddRow(row + 2, varName, index);
	}
	else
	{
		const QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName;
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

	cPushButtonAnimSound *audioSelectorButton = new cPushButtonAnimSound(table);
	table->setCellWidget(row, animSoundColumn, audioSelectorButton);
	connect(audioSelectorButton, &cPushButtonAnimSound::signalAudioSelectorClosed, this,
		&cKeyframeAnimation::slotRefreshTable);

	static_cast<cPushButtonAnimSound *>(table->cellWidget(row, animSoundColumn))
		->AssignParameterName(fullParameterName);
	static_cast<cPushButtonAnimSound *>(table->cellWidget(row, animSoundColumn))
		->AssignAnimation(keyframes);
}

int cKeyframeAnimation::AddColumn(const cAnimationFrames::sAnimationFrame &frame, int index)
{
	table->blockSignals(true);
	int newColumn = index + reservedColumns;
	if (index == -1)
	{
		newColumn = table->columnCount();
		index = newColumn - reservedColumns;
	}
	table->insertColumn(newColumn);
	table->setColumnWidth(newColumn, previewSize.width());

	const int frameNo = keyframes->GetFrameIndexForKeyframe(newColumn - reservedColumns);
	const double time = double(frameNo) / params->Get<double>("keyframe_frames_per_second");
	const int minutes = int(time / 60);
	const int seconds = int(time) % 60;
	const int milliseconds = int(time * 1000.0) % 1000;

	const QString columnHeader = QString("%1 (%2)\n(%3:%4.%5)")
																 .arg(newColumn - reservedColumns)
																 .arg(frameNo)
																 .arg(minutes)
																 .arg(seconds, 2, 10, QChar('0'))
																 .arg(milliseconds, 3, 10, QChar('0'));
	table->setHorizontalHeaderItem(newColumn, new QTableWidgetItem(columnHeader));

	table->setItem(framesPerKeyframeRow, newColumn,
		new QTableWidgetItem(QString::number(frame.numberOfSubFrames)));

	int nextIndex = min(index + 1, keyframes->GetNumberOfFrames() - 1);
	const cAnimationFrames::sAnimationFrame &nextFrame = keyframes->GetFrame(nextIndex);
	double cameraSpeed = GetCameraSpeed(frame, nextFrame);
	table->setItem(cameraSpeedRow, newColumn, new QTableWidgetItem(QString::number(cameraSpeed)));
	table->item(cameraSpeedRow, newColumn)->setFlags(Qt::NoItemFlags);

	QList<cAnimationFrames::sParameterDescription> parList = keyframes->GetListOfUsedParameters();

	using namespace parameterContainer;
	for (int i = 0; i < parList.size(); ++i)
	{
		const QString parameterName = parList[i].containerName + "_" + parList[i].parameterName;
		const enumVarType type = parList[i].varType;
		const int row = parameterRows[i];

		cOneParameter parameter = frame.parameters.GetAsOneParameter(parameterName);
		const parameterContainer::enumMorphType morphType = parameter.GetMorphType();

		if (type == typeVector3)
		{
			const CVector3 val = parameter.Get<CVector3>(valueActual);
			if (parameter.IsEmpty())
			{
				table->setItem(row, newColumn, new QTableWidgetItem(""));
				table->setItem(row + 1, newColumn, new QTableWidgetItem(""));
				table->setItem(row + 2, newColumn, new QTableWidgetItem(""));
			}
			else
			{
				table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 15)));
				table->setItem(
					row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 15)));
				table->setItem(
					row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 15)));
			}
			table->item(row, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row + 1, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row + 2, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row, newColumn)->setForeground(QBrush(Qt::black));
			table->item(row + 1, newColumn)->setForeground(QBrush(Qt::black));
			table->item(row + 2, newColumn)->setForeground(QBrush(Qt::black));
		}
		else if (type == typeVector4)
		{
			const CVector4 val = parameter.Get<CVector4>(valueActual);
			if (parameter.IsEmpty())
			{
				table->setItem(row, newColumn, new QTableWidgetItem(""));
				table->setItem(row + 1, newColumn, new QTableWidgetItem(""));
				table->setItem(row + 2, newColumn, new QTableWidgetItem(""));
				table->setItem(row + 3, newColumn, new QTableWidgetItem(""));
			}
			else
			{
				table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 15)));
				table->setItem(
					row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 15)));
				table->setItem(
					row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 15)));
				table->setItem(
					row + 3, newColumn, new QTableWidgetItem(QString("%L1").arg(val.w, 0, 'g', 15)));
			}

			table->item(row, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row + 1, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row + 2, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row + 3, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row, newColumn)->setForeground(QBrush(Qt::black));
			table->item(row + 1, newColumn)->setForeground(QBrush(Qt::black));
			table->item(row + 2, newColumn)->setForeground(QBrush(Qt::black));
			table->item(row + 3, newColumn)->setForeground(QBrush(Qt::black));
		}
		else if (type == typeRgb)
		{
			const sRGB val = parameter.Get<sRGB>(valueActual);
			if (parameter.IsEmpty())
			{
				table->setItem(row, newColumn, new QTableWidgetItem(""));
				table->setItem(row + 1, newColumn, new QTableWidgetItem(""));
				table->setItem(row + 2, newColumn, new QTableWidgetItem(""));
			}
			else
			{
				table->setItem(row, newColumn, new QTableWidgetItem(QString::number(val.R)));
				table->setItem(row + 1, newColumn, new QTableWidgetItem(QString::number(val.G)));
				table->setItem(row + 2, newColumn, new QTableWidgetItem(QString::number(val.B)));
			}

			table->item(row, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row + 1, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row + 2, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row, newColumn)->setForeground(QBrush(Qt::black));
			table->item(row + 1, newColumn)->setForeground(QBrush(Qt::black));
			table->item(row + 2, newColumn)->setForeground(QBrush(Qt::black));
		}
		else
		{
			const QString val = parameter.Get<QString>(valueActual);
			if (parameter.IsEmpty())
			{
				table->setItem(row, newColumn, new QTableWidgetItem(""));
			}
			else
			{
				table->setItem(row, newColumn, new QTableWidgetItem(val));
			}

			table->item(row, newColumn)->setBackground(QBrush(MorphType2Color(morphType)));
			table->item(row, newColumn)->setForeground(QBrush(Qt::black));
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
		case morphNone: color = QColor(255, 255, 255); break;						 // white
		case morphLinear: color = QColor(200, 200, 200); break;					 // gray
		case morphLinearAngle: color = QColor(200, 200, 255); break;		 // gray/blue
		case morphCatMullRom: color = QColor(200, 255, 200); break;			 // green
		case morphCatMullRomAngle: color = QColor(200, 255, 255); break; // green/blue
		case morphAkima: color = QColor(255, 200, 200); break;					 // red
		case morphAkimaAngle: color = QColor(255, 200, 255); break;			 // red/blue
		case morphCubic: color = QColor(255, 255, 200); break;					 // yellow
		case morphCubicAngle: color = QColor(255, 200, 255); break;			 // yellow/blue
		case morphSteffen: color = QColor(150, 150, 200); break;				 // blue
		case morphSteffenAngle: color = QColor(150, 150, 255); break;		 // blue
	}
	return color;
}

std::shared_ptr<cRenderJob> cKeyframeAnimation::PrepareRenderJob(bool *stopRequest)
{
	// preparing Render Job
	std::shared_ptr<cRenderJob> renderJob(
		new cRenderJob(params, fractalParams, image, 1, stopRequest, imageWidget));
	connect(renderJob.get(),
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(renderJob.get(), SIGNAL(updateStatistics(cStatistics)), this,
		SIGNAL(updateStatistics(cStatistics)));
	if (!systemData.noGui)
	{
		connect(renderJob.get(), SIGNAL(updateImage()), mainInterface->renderedImage, SLOT(update()));
		connect(renderJob.get(), SIGNAL(sendRenderedTilesList(QList<sRenderedTileData>)),
			mainInterface->renderedImage, SLOT(showRenderedTilesList(QList<sRenderedTileData>)));
	}
	return renderJob;
}

bool cKeyframeAnimation::InitFrameRanges(sFrameRanges *frameRanges)
{
	// range of keyframes to render
	frameRanges->startFrame = params->Get<int>("keyframe_first_to_render");
	frameRanges->endFrame = params->Get<int>("keyframe_last_to_render");
	frameRanges->totalFrames = keyframes->GetTotalNumberOfFrames();
	if (frameRanges->totalFrames < 1) frameRanges->totalFrames = 1;

	if (frameRanges->endFrame == 0) frameRanges->endFrame = frameRanges->totalFrames;

	if (frameRanges->startFrame == frameRanges->endFrame)
	{
		emit showErrorMessage(
			QObject::tr(
				"There is no frame to render: first frame to render and last frame to render are equals."),
			cErrorMessage::warningMessage);
		return false;
	}
	return true;
}

void cKeyframeAnimation::InitFrameMarkers(const sFrameRanges &frameRanges)
{
	alreadyRenderedFrames.resize(frameRanges.totalFrames);
	alreadyRenderedFrames.fill(false);
	reservedFrames.clear();
	reservedFrames.resize(frameRanges.totalFrames);
	reservedFrames.fill(false);
}

void cKeyframeAnimation::VerifyAnimation(bool *stopRequest)
{
	// checking for collisions
	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		if (params->Get<bool>("keyframe_auto_validate"))
		{
			keyframes->ClearMorphCache();
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
}

void cKeyframeAnimation::CheckWhichFramesAreAlreadyRendered(const sFrameRanges &frameRanges)
{
	// Check if frames have already been rendered

	for (int frameNo = 0; frameNo < keyframes->GetTotalNumberOfFrames(); frameNo++)
	{
		int index = keyframes->GetKeyframeIndex(frameNo);
		int subIndex = keyframes->GetSubIndex(frameNo);

		const QString filename = GetKeyframeFilename(index, subIndex, false);

		alreadyRenderedFrames[frameNo] = (QFile(filename).exists() || frameNo < frameRanges.startFrame
																			|| frameNo >= frameRanges.endFrame);
		if (gNetRender->IsClient())
		{
			if (netRenderListOfFramesToRender.size() > 0)
			{
				if (frameNo < netRenderListOfFramesToRender[0]) alreadyRenderedFrames[frameNo] = true;
			}
		}
		reservedFrames[frameNo] = alreadyRenderedFrames[frameNo];
	}
}

bool cKeyframeAnimation::AllFramesAlreadyRendered(
	const sFrameRanges &frameRanges, bool *startRenderKeyframesAgain)
{
	bool result = true;

	if (!gNetRender->IsClient())
	{
		if (keyframes->GetNumberOfFrames() - 1 > 0 && frameRanges.unrenderedTotalBeforeRender == 0)
		{
			bool deletePreviousRender;
			const QString questionTitle = QObject::tr("Truncate Image Folder");
			const QString questionText =
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
				cAnimationFrames::WipeFramesFromFolder(params->Get<QString>("anim_keyframe_dir"));
				if (!systemData.noGui && image->IsMainImage())
				{
					mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
					imageWidget->SetEnableClickModes(true);
				}
				*startRenderKeyframesAgain = true;
			}
			else
			{
				result = false;
			}
		}
	}

	return result;
}

void cKeyframeAnimation::InitJobsForClients(const sFrameRanges &frameRanges)
{
	int numberOfFramesForNetRender;
	if (gNetRender->GetClientCount() == 0)
	{
		numberOfFramesForNetRender = 0;
	}
	else
	{
		numberOfFramesForNetRender =
			frameRanges.unrenderedTotalBeforeRender / gNetRender->GetClientCount() / 2 + 1;
		if (numberOfFramesForNetRender < minFramesForNetRender)
			numberOfFramesForNetRender = minFramesForNetRender;
	}
	if (numberOfFramesForNetRender > maxFramesForNetRender)
		numberOfFramesForNetRender = maxFramesForNetRender;

	qint32 renderId = rand();
	gNetRender->SetCurrentRenderId(renderId);
	gNetRender->SetAnimation(true);
	int frameIndex = 0;
	for (int i = 0; i < gNetRender->GetClientCount(); i++)
	{
		QList<int> startingFrames;
		for (int j = 0; j < numberOfFramesForNetRender; j++)
		{
			// looking for next unrendered frame
			bool notFound = false;
			while (alreadyRenderedFrames[frameIndex])
			{
				frameIndex++;
				if (frameIndex >= frameRanges.totalFrames)
				{
					notFound = true;
					break;
				}
			}
			if (notFound)
			{
				break;
			}
			else
			{
				startingFrames.append(frameIndex);
				reservedFrames[frameIndex] = true;
				frameIndex++;
			}
		}
		if (startingFrames.size() > 0)
		{
			emit SendNetRenderSetup(i, startingFrames);
		}
	}
	emit NetRenderCurrentAnimation(params, fractalParams, false);
}

void cKeyframeAnimation::UpdateCameraAndTarget()
{
	// recalculation of camera rotation and distance (just for display purposes)
	const CVector3 camera = params->Get<CVector3>("camera");
	const CVector3 target = params->Get<CVector3>("target");
	const CVector3 top = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, top);
	params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
	params->Set("camera_distance_to_target", cameraTarget.GetDistance());

	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

		// show distance in statistics table
		const double distance =
			mainInterface->GetDistanceForPoint(params->Get<CVector3>("camera"), params, fractalParams);
		mainInterface->mainWindow->GetWidgetDockStatistics()->UpdateDistanceToFractal(distance);
	}
}

void cKeyframeAnimation::ConfirmAndSendRenderedFrames(
	const int frameIndex, const QStringList &listOfSavedFiles)
{
	emit NetRenderConfirmRendered(frameIndex, netRenderListOfFramesToRender.size());
	netRenderListOfFramesToRender.removeAll(frameIndex);
	emit NetRenderNotifyClientStatus();
	for (QString channelFileName : listOfSavedFiles)
	{
		emit NetRenderAddFileToSender(channelFileName);
	}
}

void cKeyframeAnimation::UpadeProgressInformation(
	const sFrameRanges &frameRanges, cProgressText *progressText, const int frameIndex, int index)
{
	double percentDoneFrame;
	if (frameRanges.unrenderedTotalBeforeRender > 0)
		percentDoneFrame = double(renderedFramesCount) / frameRanges.unrenderedTotalBeforeRender;
	else
		percentDoneFrame = 1.0;

	const QString progressTxt = progressText->getText(percentDoneFrame);
	emit updateProgressAndStatus(QObject::tr("Rendering animation"),
		QObject::tr("Frame %1 of %2 (key %3)").arg(frameIndex).arg(frameRanges.totalFrames).arg(index)
			+ " " + progressTxt,
		percentDoneFrame, cProgressText::progress_ANIMATION);
}

bool cKeyframeAnimation::RenderKeyframes(bool *stopRequest)
{
	mainInterface->DisablePeriodicRefresh();

	if (image->IsUsed())
	{
		emit showErrorMessage(
			QObject::tr(
				"Rendering engine is busy. Stop unfinished rendering before starting rendering animation"),
			cErrorMessage::errorMessage);
		return false;
	}

	*stopRequest = false;
	animationStopRequest = false;

	// preparing Render Job
	std::shared_ptr<cRenderJob> renderJob = PrepareRenderJob(stopRequest);

	cRenderingConfiguration config;

	if (systemData.noGui)
	{
		config.DisableRefresh();
	}
	config.EnableNetRender();
	config.DisableProgressiveRender();

	renderJob->Init(cRenderJob::keyframeAnim, config);

	cProgressText progressText;
	progressText.ResetTimer();

	keyframes->UpdateFramesIndexesTable();

	// range of keyframes to render
	sFrameRanges frameRanges;
	if (!InitFrameRanges(&frameRanges)) return false;

	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		mainInterface->mainWindow->GetWidgetDockNavigation()->LockAllFunctions();
		imageWidget->SetEnableClickModes(false);
	}

	InitFrameMarkers(frameRanges);

	try
	{
		// updating parameters
		if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
		{
			mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
			gUndo->Store(params, fractalParams, nullptr, keyframes);

			cSettings parSettings(cSettings::formatCondensedText);
			parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
			parSettings.SaveToFile(systemDirectories.GetHistoryFileName());
		}

		keyframes->RefreshAllAudioTracks(params);

		// checking for collisions
		VerifyAnimation(stopRequest);

		// Check if frames have already been rendered
		CheckWhichFramesAreAlreadyRendered(frameRanges);

		// count number of unrendered frames
		frameRanges.unrenderedTotalBeforeRender = 0;
		for (int i = 0; i < frameRanges.totalFrames; i++)
		{
			if (!alreadyRenderedFrames[i]) frameRanges.unrenderedTotalBeforeRender++;
		}

		// message if all frames are already rendered
		bool startRenderKeyframesAgain = false;
		bool result = AllFramesAlreadyRendered(frameRanges, &startRenderKeyframesAgain);

		if (startRenderKeyframesAgain) return RenderKeyframes(stopRequest);
		if (!result) throw false;

		animationIsRendered = true;

		if (gNetRender->IsServer())
		{
			InitJobsForClients(frameRanges);
		}

		keyframes->ClearMorphCache();

		// main loop for rendering of frames
		renderedFramesCount = 0;
		for (int frameIndex = 0; frameIndex < keyframes->GetTotalNumberOfFrames(); frameIndex++)
		{
			int index = keyframes->GetKeyframeIndex(frameIndex);
			int subIndex = keyframes->GetSubIndex(frameIndex);

			params->Set("frame_no", frameIndex);

			// skip already rendered frame
			if (alreadyRenderedFrames[frameIndex]) continue;
			if (reservedFrames[frameIndex]) continue;

			if (gNetRender->IsClient())
			{
				//					//waiting for new jobs when list is empty
				//					while (gNetRender->IsClient() && !animationStopRequest &&
				// netRenderListOfFramesToRender.size() == 0)
				//					{
				//						gApplication->processEvents();
				//					}

				bool frameFound = false;
				for (int f = 0; f < netRenderListOfFramesToRender.size(); f++)
				{
					if (netRenderListOfFramesToRender[f] == frameIndex)
					{
						frameFound = true;
						break;
					}
				}
				if (!frameFound) continue;
			}

			if (gNetRender->IsServer())
			{
				reservedFrames[frameIndex] = true;
			}

			UpadeProgressInformation(frameRanges, &progressText, frameIndex, index);

			if (*stopRequest || systemData.globalStopRequest || animationStopRequest) throw false;

			keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, params, fractalParams);

			cScripts::EvaluateAll(params, fractalParams);

			// recalculation of camera rotation and distance (just for display purposes)
			UpdateCameraAndTarget();

			// render frame
			renderJob->UpdateParameters(params, fractalParams);
			result = renderJob->Execute();
			if (!result) throw false;

			// save frame
			QStringList listOfSavedFiles;
			const QString filename = GetKeyframeFilename(index, subIndex, gNetRender->IsClient());

			if (gNetRender->IsClient())
			{
				const ImageFileSave::enumImageFileType fileType =
					ImageFileSave::enumImageFileType(params->Get<int>("keyframe_animation_image_type"));
				listOfSavedFiles = SaveImage(filename, fileType, image, gMainInterface->mainWindow);
			}
			else
			{
				std::shared_ptr<cImage> imageCopy(new cImage(*image.get()));

				const ImageFileSave::enumImageFileType fileType =
					ImageFileSave::enumImageFileType(params->Get<int>("keyframe_animation_image_type"));

				cKeyframeSaveImageThread *savingThread =
					new cKeyframeSaveImageThread(imageCopy, filename, fileType);

				QThread *thread = new QThread();
				thread->setObjectName("Image saving thread");

				connect(thread, &QThread::started, savingThread, &cKeyframeSaveImageThread::startSaving);

				connect(savingThread, &cKeyframeSaveImageThread::savingFinished, savingThread,
					&cKeyframeSaveImageThread::deleteLater);
				connect(savingThread, &cKeyframeSaveImageThread::savingFinished, thread, &QThread::quit);
				connect(thread, &QThread::finished, thread, &QThread::deleteLater);

				savingThread->moveToThread(thread);
				thread->start();
			}

			renderedFramesCount++;
			alreadyRenderedFrames[frameIndex] = true;

			// qDebug() << "Finished rendering frame" << frameIndex;

			if (gNetRender->IsClient())
			{
				ConfirmAndSendRenderedFrames(frameIndex, listOfSavedFiles);
			}

			gApplication->processEvents();
		}
		//--------------------------------------------------------------------

		emit updateProgressAndStatus(QObject::tr("Animation finished"), progressText.getText(1.0), 1.0,
			cProgressText::progress_IMAGE);
		emit updateProgressHide();
		emit notifyRenderKeyframeRenderStatus(
			QObject::tr("Animation finished"), progressText.getText(1.0));
	}
	catch (bool ex)
	{
		if (gNetRender->IsServer())
		{
			emit NetRenderStopAllClients();
		}

		if (gNetRender->IsClient())
		{
			gNetRender->SetStatus(netRenderSts_READY);
			emit NetRenderNotifyClientStatus();
		}

		QString resultStatus = QObject::tr("Rendering terminated");
		if (ex) resultStatus += " - " + QObject::tr("Error occured, see log output");
		emit updateProgressAndStatus(
			resultStatus, progressText.getText(1.0), cProgressText::progress_ANIMATION);
		emit updateProgressHide();

		if (!systemData.noGui && image->IsMainImage())
		{
			mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
			imageWidget->SetEnableClickModes(true);
		}
		animationIsRendered = false;
		return false;
	}

	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
		imageWidget->SetEnableClickModes(true);
	}

	if (gNetRender->IsClient())
	{
		gNetRender->SetStatus(netRenderSts_READY);
		emit NetRenderNotifyClientStatus();
	}

	animationIsRendered = false;
	return true;
}

void cKeyframeAnimation::RefreshTable()
{
	keyframes->RemoveMissingParameters(params, fractalParams);

	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);

	keyframes->SetLooped(params->Get<bool>("keyframe_animation_loop"));

	if (keyframes->GetNumberOfFrames() < 3) keyframes->SetLooped(false);

	keyframes->UpdateFramesIndexesTable();
	UpdateLimitsForFrameRange(); // it is needed to do it also here, because limits must be set just
															 // after loading of settings

	keyframes->UpdateFramesIndexesTable();

	keyframes->RefreshAllAudioTracks(params);

	int lastSelectedColumn = table->currentColumn();

	PrepareTable();
	gApplication->processEvents();

	const int noOfKeyframes = keyframes->GetNumberOfFrames();

	auto tempPar = std::make_shared<cParameterContainer>();
	*tempPar = *params;
	auto tempFract = std::make_shared<cFractalContainer>();
	*tempFract = *fractalParams;

	for (int i = 0; i < noOfKeyframes; i++)
	{
		const int newColumn = AddColumn(keyframes->GetFrame(i));

		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			double dpiScale = table->devicePixelRatioF();
			cThumbnailWidget *thumbWidget =
				new cThumbnailWidget(previewSize.width(), previewSize.height(), dpiScale, table);
			thumbWidget->UseOneCPUCore(true);
			keyframes->GetFrameAndConsolidate(i, tempPar, tempFract);
			tempPar->Set("frame_no", keyframes->GetFrameIndexForKeyframe(i));
			thumbWidget->AssignParameters(tempPar, tempFract);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		if (i % 100 == 0)
		{
			emit updateProgressAndStatus(QObject::tr("Refreshing animation"),
				tr("Refreshing animation frames"), double(i) / noOfKeyframes,
				cProgressText::progress_ANIMATION);
			gApplication->processEvents();
		}

		if (systemData.globalStopRequest) break;
	}

	table->selectColumn(lastSelectedColumn);

	UpdateLimitsForFrameRange();

	UpdateAnimationPathCameraAndLights();
	UpdateAnimationPathSingleParameter();

	mainInterface->progressBarAnimation->hide();
}

QString cKeyframeAnimation::GetParameterName(int rowNumber)
{
	const int parameterNumber = rowParameter[rowNumber];

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
	const CVector3 camera = params->Get<CVector3>("camera");
	const CVector3 target = params->Get<CVector3>("target");
	const CVector3 top = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, top);
	params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
	params->Set("camera_distance_to_target", cameraTarget.GetDistance());

	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

	params->Set("frame_no", keyframes->GetFrameIndexForKeyframe(index));

	mainInterface->StartRender();
}

void cKeyframeAnimation::DeleteFramesFrom(int index) const
{
	gUndo->Store(params, fractalParams, nullptr, keyframes);
	for (int i = keyframes->GetNumberOfFrames() - 1; i >= index; i--)
		table->removeColumn(index + reservedColumns);
	keyframes->DeleteFrames(index, keyframes->GetNumberOfFrames() - 1);

	keyframes->UpdateFramesIndexesTable();

	UpdateLimitsForFrameRange();
}

void cKeyframeAnimation::DeleteFramesTo(int index) const
{
	gUndo->Store(params, fractalParams, nullptr, keyframes);
	for (int i = 0; i <= index; i++)
		table->removeColumn(reservedColumns);
	keyframes->DeleteFrames(0, index);

	keyframes->UpdateFramesIndexesTable();

	UpdateLimitsForFrameRange();
}

void cKeyframeAnimation::slotSelectKeyframeAnimImageDir() const
{
	QFileDialog *dialog = new QFileDialog();
	dialog->setFileMode(QFileDialog::Directory);
	dialog->setNameFilter(QObject::tr("Animation Image Folder"));
	dialog->setDirectory(QDir::toNativeSeparators(params->Get<QString>("anim_keyframe_dir")));
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setWindowTitle(QObject::tr("Choose Animation Image Folder"));
	dialog->setOption(QFileDialog::ShowDirsOnly);

	if (dialog->exec())
	{
		QStringList fileNames = dialog->selectedFiles();
		const QString filename = QDir::toNativeSeparators(fileNames.first() + QDir::separator());
		ui->text_anim_keyframe_dir->setText(filename);
		params->Set("anim_keyframe_dir", filename);
	}
}

void cKeyframeAnimation::slotTableCellChanged(int row, int column)
{
	table->blockSignals(true);

	if (row >= reservedRows)
	{
		QTableWidgetItem *cell = table->item(row, column);
		QString cellText = cell->text();

		const int index = column - reservedColumns;
		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);

		const QString parameterName = GetParameterName(row);
		const int parameterFirstRow = parameterRows[rowParameter[row]];
		const int vectIndex = row - parameterFirstRow;

		if (cellText.isEmpty())
		{
			frame.parameters.SetEmpty(parameterName);
		}
		else
		{
			using namespace parameterContainer;
			const enumVarType type = frame.parameters.GetVarType(parameterName);

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
		}
		keyframes->ModifyFrame(index, frame);

		// update thumbnail
		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			auto tempPar = std::make_shared<cParameterContainer>();
			*tempPar = *params;
			auto tempFract = std::make_shared<cFractalContainer>();
			*tempFract = *fractalParams;

			keyframes->GetFrameAndConsolidate(index, tempPar, tempFract);
			cThumbnailWidget *thumbWidget = static_cast<cThumbnailWidget *>(table->cellWidget(0, column));

			if (!thumbWidget)
			{
				thumbWidget = new cThumbnailWidget(previewSize.width(), previewSize.height(), 1, table);
				thumbWidget->UseOneCPUCore(true);
				thumbWidget->AssignParameters(tempPar, tempFract);
				table->setCellWidget(0, column, thumbWidget);
			}
			else
			{
				thumbWidget->AssignParameters(tempPar, tempFract);
			}
		}
		UpdateAnimationPathCameraAndLights();
		UpdateAnimationPathSingleParameter();
	}
	else
	{
		if (row == framesPerKeyframeRow)
		{
			QTableWidgetItem *cell = table->item(row, column);
			QString cellText = cell->text();
			int newFramesPerKey = cellText.toInt();
			if (newFramesPerKey < 1) newFramesPerKey = 1;

			const int index = column - reservedColumns;
			cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);
			frame.numberOfSubFrames = newFramesPerKey;
			keyframes->ModifyFrame(index, frame);

			keyframes->UpdateFramesIndexesTable();
		}
	}

	table->blockSignals(false);

	const int index = min(column - reservedColumns, keyframes->GetNumberOfFrames() - 1);
	int nextIndex = min(index + 1, keyframes->GetNumberOfFrames() - 1);
	double speed = GetCameraSpeed(keyframes->GetFrame(index), keyframes->GetFrame(nextIndex));
	QTableWidgetItem *speedCell = table->item(cameraSpeedRow, index + reservedColumns);
	speedCell->setText(QString::number(speed));
	table->update();
}

void cKeyframeAnimation::slotDeleteAllImages() const
{
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_keyframeAnimationParameters, params, qInterface::read);

	const QMessageBox::StandardButton reply = QMessageBox::question(
		mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Truncate Image Folder"),
		QObject::tr("This will delete all images in the image folder.\nProceed?"),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		cAnimationFrames::WipeFramesFromFolder(params->Get<QString>("anim_keyframe_dir"));
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
	gUndo->Store(params, fractalParams, nullptr, keyframes);

	QTableWidgetItem *cell = table->item(row, column);
	QString cellText = cell->text();

	const QString parameterName = GetParameterName(row);

	const int index = column - reservedColumns;
	cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);

	using namespace parameterContainer;
	const enumVarType type = frame.parameters.GetVarType(parameterName);

	bool valueIsInteger = false;
	bool valueIsDouble = false;
	bool valueIsText = false;
	int valueInteger = 0;
	double valueDouble = 0.0;
	QString valueText;

	bool ok;
	const int lastFrame = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation",
		"Enter last keyframe number", index, index + 1, keyframes->GetNumberOfFrames() - 1, 1, &ok);
	if (!ok) return;

	const int numberOfFrames = (lastFrame - index);

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

	double integerStep = 0.0;
	double doubleStep = 0.0;

	if (valueIsInteger)
	{
		const int finalInteger =
			QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation",
				"Enter value for last keyframe", valueInteger, 0, 2147483647, 1, &ok);
		integerStep = double(finalInteger - valueInteger) / numberOfFrames;
	}
	else if (valueIsDouble)
	{
		const double finalDouble = systemData.locale.toDouble(QInputDialog::getText(
			mainInterface->mainWindow, "Parameter interpolation", "Enter value for last keyframe",
			QLineEdit::Normal, QString("%L1").arg(valueDouble, 0, 'g', 15), &ok));
		doubleStep = (finalDouble - valueDouble) / numberOfFrames;
	}

	if (!ok) return;

	for (int i = index + 1; i <= lastFrame; i++)
	{
		QString newCellText;
		if (valueIsInteger)
		{
			const int newValue = int(integerStep * (i - index) + valueInteger);
			newCellText = QString::number(newValue);
		}
		else if (valueIsDouble)
		{
			const double newValue = doubleStep * (i - index) + valueDouble;
			newCellText = QString("%L1").arg(newValue, 0, 'g', 15);
		}
		else if (valueIsText)
		{
			newCellText = valueText;
		}
		QTableWidgetItem *newCell = table->item(row, i + reservedColumns);
		newCell->setText(newCellText);
		newCell->setForeground(QBrush(Qt::black));
	}
}

void cKeyframeAnimation::CopyToAllKeyframes(int row, int column)
{
	QTableWidgetItem *cell = table->item(row, column);
	QString cellText = cell->text();

	for (int i = reservedColumns; i < table->columnCount(); i++)
	{
		table->item(row, i)->setText(cellText);
	}
}

void cKeyframeAnimation::slotRefreshTable()
{
	RefreshTable();
}

QString cKeyframeAnimation::GetKeyframeFilename(int index, int subIndex, bool netRenderCache) const
{
	const int frameIndex = keyframes->GetFrameIndexForKeyframe(index) + subIndex;

	QString dir;
	if (netRenderCache)
	{
		dir = systemDirectories.GetNetrenderFolder() + QDir::separator()
					+ QString("pid%1_").arg(QCoreApplication::applicationPid());
	}
	else
	{
		dir = params->Get<QString>("anim_keyframe_dir");
	}

	QString filename = dir + "frame_" + QString("%1").arg(frameIndex, 7, 10, QChar('0'));
	filename += "."
							+ ImageFileSave::ImageFileExtension(ImageFileSave::enumImageFileType(
								params->Get<int>("keyframe_animation_image_type")));
	return filename;
}

parameterContainer::enumMorphType cKeyframeAnimation::GetMorphType(int row) const
{
	const int parameterIndex = rowParameter.at(row);
	const parameterContainer::enumMorphType morphType =
		keyframes->GetListOfParameters().at(parameterIndex).morphType;
	return morphType;
}

void cKeyframeAnimation::ChangeMorphType(int row, parameterContainer::enumMorphType morphType)
{
	gUndo->Store(params, fractalParams, nullptr, keyframes);
	const int parameterIndex = rowParameter.at(row);
	keyframes->ChangeMorphType(parameterIndex, morphType);
	RefreshTable();
}

void cKeyframeAnimation::slotExportKeyframesToFlight()
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo->Store(params, fractalParams, gAnimFrames, keyframes);

	keyframes->UpdateFramesIndexesTable();

	if (gAnimFrames->GetFrames().size() > 0)
	{
		const QMessageBox::StandardButton reply = QMessageBox::question(
			mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Export keyframes to flight"),
			QObject::tr(
				"There are already captured flight frames present.\nDiscard current flight frames ?"),
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::No) return;
	}

	gAnimFrames->ClearAll();
	gAnimFrames->SetListOfParametersAndClear(keyframes->GetListOfParameters(), params);

	keyframes->ClearMorphCache();

	keyframes->RefreshAllAudioTracks(params);

	for (int frameIndex = 0; frameIndex < keyframes->GetTotalNumberOfFrames(); frameIndex++)
	{
		int index = keyframes->GetKeyframeIndex(frameIndex);
		gAnimFrames->AddFrame(keyframes->GetInterpolatedFrame(frameIndex, params, fractalParams));

		if (frameIndex % 100 == 0)
		{
			emit updateProgressAndStatus(QObject::tr("Exporting"), tr("Exporting keyframes to flight"),
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
	int noOfFrames = (keyframes->GetTotalNumberOfFrames() - 1);
	if (noOfFrames < 0) noOfFrames = 0;

	ui->spinboxInt_keyframe_first_to_render->setMaximum(noOfFrames);

	ui->spinboxInt_keyframe_last_to_render->setMaximum(noOfFrames);
	ui->horizontalSlider_actualFrame->setMaximum(noOfFrames);

	if (lastToRenderMax)
	{
		ui->spinboxInt_keyframe_last_to_render->setValue(noOfFrames);
	}
}

void cKeyframeAnimation::slotMovedSliderFirstFrame() const
{
	int value = ui->spinboxInt_keyframe_first_to_render->value();
	if (value > ui->spinboxInt_keyframe_last_to_render->value())
		ui->spinboxInt_keyframe_last_to_render->setValue(value);
}
void cKeyframeAnimation::slotMovedSliderLastFrame()
{
	int value = ui->spinboxInt_keyframe_last_to_render->value();
	lastToRenderMax = (value == ui->spinboxInt_keyframe_last_to_render->maximum());
	if (value < ui->spinboxInt_keyframe_first_to_render->value())
		ui->spinboxInt_keyframe_first_to_render->setValue(value);
}

QList<int> cKeyframeAnimation::CheckForCollisions(double minDist, bool *stopRequest)
{
	QList<int> listOfCollisions;

	auto tempPar = std::make_shared<cParameterContainer>();
	*tempPar = *params;
	auto tempFractPar = std::make_shared<cFractalContainer>();
	*tempFractPar = *fractalParams;

	*stopRequest = false;

	keyframes->UpdateFramesIndexesTable();

	keyframes->ClearMorphCache();

	for (int frameIndex = 0; frameIndex < keyframes->GetTotalNumberOfFrames(); frameIndex++)
	{
		int index = keyframes->GetKeyframeIndex(frameIndex);

		gApplication->processEvents();
		if (*stopRequest || systemData.globalStopRequest) return listOfCollisions;

		keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, tempPar, tempFractPar);
		tempPar->Set("frame_no", frameIndex);
		const CVector3 point = tempPar->Get<CVector3>("camera");
		const double dist = mainInterface->GetDistanceForPoint(point, tempPar, tempFractPar);
		if (dist < minDist)
		{
			listOfCollisions.append(frameIndex);
		}

		if (frameIndex % 100 == 0)
		{
			emit updateProgressAndStatus(QObject::tr("Checking for collisions"),
				QObject::tr("Checking for collisions on keyframe # %1").arg(index),
				double(frameIndex) / keyframes->GetTotalNumberOfFrames(),
				cProgressText::progress_ANIMATION);
		}
	}

	emit updateProgressAndStatus(QObject::tr("Checking for collisions"),
		QObject::tr("Checking for collisions finished"), 1.0, cProgressText::progress_ANIMATION);

	return listOfCollisions;
}

void cKeyframeAnimation::slotValidate()
{
	// updating parameters
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo->Store(params, fractalParams, nullptr, keyframes);

	// checking for collisions
	QList<int> listOfCollisions = CheckForCollisions(
		params->Get<double>("keyframe_collision_thresh"), &mainInterface->stopRequest);
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

void cKeyframeAnimation::slotCellDoubleClicked(int row, int column)
{
	if (row == 0)
	{
		int frameIndex = keyframes->GetFrameIndexForKeyframe(column - reservedColumns);
		ui->horizontalSlider_actualFrame->setValue(frameIndex);
		ui->widgetValueChart->slotSetCurrentFrame(frameIndex);
		updateFrameIndexLabel(frameIndex);
		RenderFrame(column - reservedColumns);
	}
}

void cKeyframeAnimation::slotCellClicked(int row, int column) const
{
	Q_UNUSED(column);
	UpdateCameraDistanceInformation();

	if (row >= reservedRows)
	{
		const int parameterIndex = rowParameter.at(row);
		int vectorComponentIndex = row - parameterRows.at(parameterIndex);
		UpdateAnimationPathSingleParameter(table->currentRow());
	}

	ui->widgetValueChart->setVisible(row > cameraSpeedRow);
}

void cKeyframeAnimation::UpdateAnimationPathSingleParameter() const
{
	if (table->currentRow() >= reservedRows)
	{
		const int parameterIndex = rowParameter.at(table->currentRow());
		const int vectorComponentIndex = table->currentRow() - parameterRows.at(parameterIndex);
		UpdateAnimationPathSingleParameter(table->currentRow());
	}
}

void cKeyframeAnimation::slotSetConstantTargetDistance()
{
	// updating parameters
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo->Store(params, fractalParams, nullptr, keyframes);

	const double constDist = params->Get<double>("keyframe_constant_target_distance");

	for (int key = 0; key < keyframes->GetNumberOfFrames() - 1; key++)
	{
		cAnimationFrames::sAnimationFrame keyframe = keyframes->GetFrame(key);

		if (keyframe.parameters.IfExists("main_camera") && keyframe.parameters.IfExists("main_target")
				&& keyframe.parameters.IfExists("main_camera_top"))
		{
			const CVector3 camera = keyframe.parameters.Get<CVector3>("main_camera");
			const CVector3 target = keyframe.parameters.Get<CVector3>("main_target");
			const CVector3 top = keyframe.parameters.Get<CVector3>("main_camera_top");

			cCameraTarget cameraTarget(camera, target, top);
			const CVector3 forwardVector = cameraTarget.GetForwardVector();

			const CVector3 newTarget = camera + forwardVector * constDist;

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
	const int newColumn = table->columnCount();
	table->insertColumn(newColumn);
	table->setHorizontalHeaderItem(newColumn, new QTableWidgetItem(tr("Audio")));
}

void cKeyframeAnimation::UpdateAnimationPathCameraAndLights() const
{
	int numberOfKeyframes = keyframes->GetNumberOfFrames();

	auto tempPar = std::make_shared<cParameterContainer>();
	*tempPar = *params;
	auto tempFractPar = std::make_shared<cFractalContainer>();
	*tempFractPar = *fractalParams;

	sAnimationPathData animationPathData;
	animationPathData.numberOfKeyframes = numberOfKeyframes;
	animationPathData.numberOfFrames = keyframes->GetTotalNumberOfFrames();
	animationPathData.actualSelectedFrameNo = table->currentColumn() - reservedColumns;
	if (animationPathData.actualSelectedFrameNo < 0) animationPathData.actualSelectedFrameNo = 0;
	animationPathData.cameraPathEnable = params->Get<bool>("show_camera_path");
	animationPathData.targetPathEnable = params->Get<bool>("show_target_path");

	for (int i = 1; i <= 4; i++)
	{
		if (params->IfExists(cLight::Name("is_defined", i)))
		{
			animationPathData.lightPathEnable[i - 1] =
				params->Get<bool>("show_light_path", i) && params->Get<bool>(cLight::Name("enabled", i));
		}
		else
		{
			animationPathData.lightPathEnable[i - 1] = false;
		}
	}

	for (int key = 0; key < keyframes->GetNumberOfFrames(); key++)
	{
		animationPathData.framesPeyKey.append(keyframes->GetFrame(key).numberOfSubFrames);
	}

	keyframes->ClearMorphCache();

	for (int frameIndex = 0; frameIndex < keyframes->GetTotalNumberOfFrames(); frameIndex++)
	{
		keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, tempPar, tempFractPar);
		sAnimationPathPoint point;
		point.camera = tempPar->Get<CVector3>("camera");
		point.target = tempPar->Get<CVector3>("target");
		CVector3 top = tempPar->Get<CVector3>("camera_top");
		cCameraTarget cameraTarget(point.camera, point.target, top);

		for (int l = 0; l < 4; l++)
		{
			if (params->IfExists(cLight::Name("is_defined", l + 1)))
			{
				if (params->Get<bool>(cLight::Name("relative_position", l + 1)))
				{
					CVector3 deltaPosition = tempPar->Get<CVector3>(cLight::Name("position", l + 1));
					CVector3 deltaPositionRotated = cameraTarget.GetForwardVector() * deltaPosition.z
																					+ cameraTarget.GetTopVector() * deltaPosition.y
																					+ cameraTarget.GetRightVector() * deltaPosition.x;
					point.lights[l] = point.camera + deltaPositionRotated;
				}
				else
				{
					point.lights[l] = tempPar->Get<CVector3>(cLight::Name("position", l + 1));
				}
				sRGB color16 = tempPar->Get<sRGB>(cLight::Name("color", l + 1));
				sRGB8 color8(quint8(color16.R / 256), quint8(color16.G / 256), quint8(color16.B / 256));
				point.lightColor[l] = color8;
			}
		}
		animationPathData.animationPath.append(point);
	}
	imageWidget->SetAnimationPath(animationPathData);
	imageWidget->update();
}

void cKeyframeAnimation::UpdateAnimationPathSingleParameter(int tableRow) const
{
	const int parameterIndex = rowParameter.at(tableRow);
	const int vectorComponentIndex = tableRow - parameterRows.at(parameterIndex);

	keyframes->ClearMorphCache();
	cAnimationValueChartWidget::cAnimationPath path;

	cAnimationFrames::sParameterDescription parameterDescr =
		keyframes->GetListOfParameters().at(parameterIndex);

	QString fullParameterName = parameterDescr.containerName + "_" + parameterDescr.parameterName;

	for (int frameIndex = 0; frameIndex < keyframes->GetTotalNumberOfFrames(); frameIndex++)
	{
		int keyframe =
			keyframes->GetKeyframeIndex(frameIndex); // Get the keyframe index for the given frame index
		int subIndex =
			keyframes->GetSubIndex(frameIndex); // Get the sub-index for the given frame index

		cOneParameter oneParameter = keyframes->InterpolateSingleParameter(
			0, keyframe, parameterDescr.parameterName, fullParameterName, subIndex, frameIndex, params);

		// get vector component value
		CVector4 vect4Value = oneParameter.Get<CVector4>(valueActual);
		double vectorComponentValue = vect4Value.GetArray()[vectorComponentIndex];

		path.values.push_back(vectorComponentValue);
	}

	path.keyframeIndices = keyframes->getFramesIndexesTable();
	path.parameterName = fullParameterName;

	for (int k = 0; k < keyframes->GetNumberOfFrames(); k++)
	{
		path.emptyKeyframes.push_back(
			keyframes->GetFrame(k).parameters.GetAsOneParameter(fullParameterName).IsEmpty());
	}

	ui->widgetValueChart->SetAnimationPath(path, tableRow);
}

void cKeyframeAnimation::slotUpdateAnimationPathSelection()
{
	SynchronizeInterfaceWindow(ui->tab_keyframe_animation, params, qInterface::read);
	UpdateAnimationPathCameraAndLights();
}

void cKeyframeAnimation::UpdateActualCameraPosition(const CVector3 &cameraPosition)
{
	actualCameraPosition = cameraPosition;
	UpdateCameraDistanceInformation();
}

void cKeyframeAnimation::UpdateCameraDistanceInformation() const
{
	if (keyframes->GetNumberOfFrames() > 0)
	{
		int selectedKeyframe = table->currentColumn() - reservedColumns;
		if (selectedKeyframe < 0) selectedKeyframe = 0;

		cKeyframes::sAnimationFrame keyframe = keyframes->GetFrame(selectedKeyframe);

		if (keyframe.parameters.IfExists("main_camera"))
		{
			CVector3 camera = keyframe.parameters.Get<CVector3>("main_camera");
			double distance = (camera - actualCameraPosition).Length();
			ui->label_camera_distance_from_keyframe->setText(
				tr("Camera distance from selected keyframe: %1").arg(distance));
		}
	}
}

void cKeyframeAnimation::SetNetRenderStartingFrames(const QVector<int> &startingFrames)
{
	netRenderListOfFramesToRender.clear();
	netRenderListOfFramesToRender.append(startingFrames.toList());
}

void cKeyframeAnimation::slotNetRenderFinishedFrame(
	int clientIndex, int frameIndex, int sizeOfToDoList)
{
	Q_UNUSED(frameIndex);

	renderedFramesCount++;
	if (frameIndex < alreadyRenderedFrames.size())
	{
		alreadyRenderedFrames[frameIndex] = true;
	}

	if (!animationStopRequest && animationIsRendered)
	{
		// qDebug() << "Server: got information about finished frame" << frameIndex << sizeOfToDoList;

		// counting left frames
		int countLeft = reservedFrames.count(false);

		// calculate maximum list size
		int numberOfFramesForNetRender = countLeft / gNetRender->GetClientCount() / 2;
		if (numberOfFramesForNetRender < minFramesForNetRender)
			numberOfFramesForNetRender = minFramesForNetRender;
		if (numberOfFramesForNetRender > maxFramesForNetRender)
			numberOfFramesForNetRender = maxFramesForNetRender;

		// calculate number for frames to supplement
		int numberOfNewFrames = numberOfFramesForNetRender - sizeOfToDoList;

		if (numberOfNewFrames > 0)
		{
			QList<int> toDoList;

			// looking for not reserved frame
			for (int f = 0; f < reservedFrames.size(); f++)
			{
				if (!reservedFrames[f])
				{
					toDoList.append(f);
					reservedFrames[f] = true;
				}
				if (toDoList.size() >= numberOfNewFrames) break;
			}
			if (toDoList.size() > 0)
			{
				emit NetRenderSendFramesToDoList(clientIndex, toDoList);
			}
			// qDebug() << "Server: new toDo list" << toDoList;
		}
	}
}

void cKeyframeAnimation::slotNetRenderUpdateFramesToDo(QList<int> listOfFrames)
{
	if (animationIsRendered)
	{
		netRenderListOfFramesToRender.append(listOfFrames);
		// qDebug() << "Client: got frames toDo:" << listOfFrames;
	}
}

void cKeyframeAnimation::slotRandomize()
{
	cRandomizerDialog *randomizer = new cRandomizerDialog(); // deleted by WA_DeleteOnClose
	randomizer->setAttribute(Qt::WA_DeleteOnClose);
	QStringList listOfParameterNames;
	QList<cKeyframes::sParameterDescription> list = keyframes->GetListOfParameters();
	for (const cKeyframes::sParameterDescription &parameter : list)
	{
		QString fullParameterName = parameter.containerName + "_" + parameter.parameterName;
		listOfParameterNames.append(fullParameterName);
	}

	randomizer->AssignParameters(listOfParameterNames);
	randomizer->show();
}

void cKeyframeAnimation::slotAnimationStopRequest()
{
	animationStopRequest = true;
}

void cKeyframeAnimation::InsertKeyframeInBetween(int index)
{
	if (keyframes)
	{
		auto tempPar = std::make_shared<cParameterContainer>();
		*tempPar = *params;
		auto tempFractPar = std::make_shared<cFractalContainer>();
		*tempFractPar = *fractalParams;

		keyframes->UpdateFramesIndexesTable();
		keyframes->ClearMorphCache();

		int frameIndex = keyframes->GetFrameIndexForKeyframe(index - 1)
										 + keyframes->GetFramesPerKeyframe(index - 1) / 2;

		keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, tempPar, tempFractPar);

		// add new frame to container
		keyframes->AddFrame(
			tempPar, tempFractPar, keyframes->GetFramesPerKeyframe(index - 1) / 2, index);
		keyframes->UpdateFramesIndexesTable();

		params->Set("frame_no", keyframes->GetFrameIndexForKeyframe(index));

		// add column to table
		const int newColumn = AddColumn(keyframes->GetFrame(index), index);
		table->selectColumn(newColumn);

		if (ui->checkBox_show_keyframe_thumbnails->isChecked())
		{
			double dpiScale = table->devicePixelRatioF();
			cThumbnailWidget *thumbWidget =
				new cThumbnailWidget(previewSize.width(), previewSize.height(), dpiScale, table);
			thumbWidget->UseOneCPUCore(false);
			thumbWidget->AssignParameters(tempPar, tempFractPar);
			table->setCellWidget(0, newColumn, thumbWidget);
		}

		// change frames per keyframe in previous keyframe
		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index - 1);
		frame.numberOfSubFrames = frame.numberOfSubFrames / 2;
		keyframes->ModifyFrame(index - 1, frame);

		keyframes->UpdateFramesIndexesTable();

		UpdateLimitsForFrameRange();

		RefreshTable();

		UpdateAnimationPathCameraAndLights();
		UpdateAnimationPathSingleParameter();
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cKeyframeAnimation::DeleteRenderedFramesForKeyframe(int keyframeIndex)
{
	if (keyframeIndex < keyframes->GetNumberOfFrames() - 1)
	{
		int startFrame = keyframes->GetFrameIndexForKeyframe(keyframeIndex);
		int endFrame = keyframes->GetFrameIndexForKeyframe(keyframeIndex + 1) - 1;

		SynchronizeInterfaceWindow(
			ui->scrollAreaWidgetContents_keyframeAnimationParameters, params, qInterface::read);

		QString folder = params->Get<QString>("anim_keyframe_dir");

		const QMessageBox::StandardButton reply = QMessageBox::question(
			mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Deleting rendered frames"),
			QObject::tr("This will delete rendered frames from %1 to %2\n"
									"in the image folder.\n"
									"%3\n"
									"Proceed?")
				.arg(startFrame)
				.arg(endFrame)
				.arg(folder),
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			cAnimationFrames::WipeFramesFromFolder(folder, startFrame, endFrame);
		}
	}
}

double cKeyframeAnimation::GetCameraSpeed(const cAnimationFrames::sAnimationFrame &frame,
	const cAnimationFrames::sAnimationFrame &nextFrame)
{
	CVector3 camera = frame.parameters.Get<CVector3>("main_camera");
	CVector3 previousCamera = nextFrame.parameters.Get<CVector3>("main_camera");
	double distance = (camera - previousCamera).Length();
	double speed = distance / frame.numberOfSubFrames;
	return speed;
}

void cKeyframeAnimation::slotAddAllParameters()
{

	struct sParameterInfo
	{
		QString parameterName;
		std::shared_ptr<cParameterContainer> container;
		std::shared_ptr<cParameterContainer> containerBefore;
	};

	std::shared_ptr<cParameterContainer> parBefore(new cParameterContainer);
	*parBefore = *params;
	std::shared_ptr<cFractalContainer> parFractBefore(new cFractalContainer);
	*parFractBefore = *fractalParams;

	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);

	QList<sParameterInfo> listOfAllModifiedParameters;

	QList<QString> listOfMainParameters = params->GetListOfParameters();
	for (QString parameterName : listOfMainParameters)
	{
		if (!params->isDefaultValue(parameterName)
				&& params->GetParameterType(parameterName) == paramStandard
				&& params->GetAsOneParameter(parameterName).GetMorphType() != morphNone)
		{
			// exceptions
			if (parameterName == "image_width") continue;
			if (parameterName == "image_height") continue;
			if (parameterName == "camera_distance_to_target") continue;
			if (parameterName == "camera_rotation") continue;

			sParameterInfo par;
			par.parameterName = parameterName;
			par.container = params;
			par.containerBefore = parBefore;
			listOfAllModifiedParameters.append(par);
		}
	}

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		QList<QString> listOfFractalParameters = fractalParams->at(i)->GetListOfParameters();
		for (QString parameterName : listOfFractalParameters)
		{
			if (!fractalParams->at(i)->isDefaultValue(parameterName)
					&& fractalParams->at(i)->GetParameterType(parameterName) == paramStandard)
			{
				sParameterInfo par;
				par.parameterName = parameterName;
				par.container = fractalParams->at(i);
				par.containerBefore = parFractBefore->at(i);
				listOfAllModifiedParameters.append(par);
			}
		}
	}

	for (const sParameterInfo &parameterInfo : listOfAllModifiedParameters)
	{
		QString parameterName = parameterInfo.parameterName;
		QString containerName = parameterInfo.container->GetContainerName();
		if (keyframes->IndexOnList(parameterName, containerName) == -1)
		{
			cOneParameter parameter = parameterInfo.containerBefore->GetAsOneParameter(parameterName);
			keyframes->AddAnimatedParameter(parameterName, parameter, params);
		}
	}

	RefreshTable();
}

void cKeyframeAnimation::ModifyValueInCells(
	const QList<QTableWidgetItem *> &selectedItemsList, enumModifyMode mode)
{
	bool ok;
	const double modifier = QInputDialog::getDouble(
		mainInterface->mainWindow, "Modify values", "Modifier:", 1.0, -1e100, 1e100, 5, &ok);
	if (!ok) return;

	for (auto cell : selectedItemsList)
	{
		int row = cell->row();
		int column = cell->column();

		if (row == framesPerKeyframeRow && column >= reservedColumns)
		{
			const int index = column - reservedColumns;
			cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);

			if (mode == modifyModeMultiply)
				frame.numberOfSubFrames *= modifier;
			else if (mode == modifyModeIncrease)
				frame.numberOfSubFrames += modifier;

			keyframes->ModifyFrame(index, frame);
		}
		else if (row >= reservedRows && column >= reservedColumns)
		{
			QString cellText = cell->text();

			const int index = column - reservedColumns;
			cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(index);

			const QString parameterName = GetParameterName(row);
			const int parameterFirstRow = parameterRows[rowParameter[row]];
			const int vectIndex = row - parameterFirstRow;

			using namespace parameterContainer;
			const enumVarType type = frame.parameters.GetVarType(parameterName);

			if (type == typeVector3)
			{
				CVector3 vect = frame.parameters.Get<CVector3>(parameterName);
				if (mode == modifyModeMultiply)
				{
					if (vectIndex == 0) vect.x *= modifier;
					if (vectIndex == 1) vect.y *= modifier;
					if (vectIndex == 2) vect.z *= modifier;
				}
				else if (mode == modifyModeIncrease)
				{
					if (vectIndex == 0) vect.x += modifier;
					if (vectIndex == 1) vect.y += modifier;
					if (vectIndex == 2) vect.z += modifier;
				}
				frame.parameters.Set(parameterName, vect);
			}
			else if (type == typeVector4)
			{
				CVector4 vect = frame.parameters.Get<CVector4>(parameterName);

				if (mode == modifyModeMultiply)
				{
					if (vectIndex == 0) vect.x *= modifier;
					if (vectIndex == 1) vect.y *= modifier;
					if (vectIndex == 2) vect.z *= modifier;
					if (vectIndex == 3) vect.w *= modifier;
				}
				else if (mode == modifyModeIncrease)
				{
					if (vectIndex == 0) vect.x += modifier;
					if (vectIndex == 1) vect.y += modifier;
					if (vectIndex == 2) vect.z += modifier;
					if (vectIndex == 3) vect.w += modifier;
				}
				frame.parameters.Set(parameterName, vect);
			}
			else
			{
				if (mode == modifyModeMultiply)
				{
					frame.parameters.Set(
						parameterName, frame.parameters.Get<double>(parameterName) * modifier);
				}
				else if (mode == modifyModeIncrease)
				{
					frame.parameters.Set(
						parameterName, frame.parameters.Get<double>(parameterName) + modifier);
				}
			}

			keyframes->ModifyFrame(index, frame);
		}
	}
	RefreshTable();
}

void cKeyframeAnimation::slotSetFramesPerKeyframeToAllKeyframes(void)
{
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_keyframeAnimationParameters, params, qInterface::read);
	int framesPerKeyframe = params->Get<int>("frames_per_keyframe");

	for (int i = 0; i < keyframes->GetNumberOfFrames(); i++)
	{
		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(i);
		frame.numberOfSubFrames = framesPerKeyframe;
		keyframes->ModifyFrame(i, frame);
	}
	RefreshTable();
}

void cKeyframeAnimation::AddAnimatedParameter(
	const QString &parameterName, std::shared_ptr<cParameterContainer> parameterContainer)
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	keyframes->AddAnimatedParameter(
		parameterName, parameterContainer->GetAsOneParameter(parameterName));
}

void cKeyframeAnimation::updateFrameIndexLabel(int frameIndex)
{
	QFontMetrics fm(ui->tab_keyframe_animation->font());
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	int textWidth = fm.horizontalAdvance(tr("Frame: 000000"));
#else
	int textWidth = fm.width(tr("Frame: 000000"));
#endif
	ui->label_actualFrame->setFixedWidth(textWidth);
	ui->label_actualFrame->setText(tr("Frame: %1").arg(frameIndex));
}

void cKeyframeAnimation::slotSliderMovedActualFrame(int frameIndex)
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	keyframes->ClearMorphCache();

	int index = keyframes->GetKeyframeIndex(frameIndex);
	int subIndex = keyframes->GetSubIndex(frameIndex);

	params->Set("frame_no", frameIndex);

	keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, params, fractalParams);

	cScripts::EvaluateAll(params, fractalParams);

	// recalculation of camera rotation and distance (just for display purposes)
	UpdateCameraAndTarget();

	table->selectColumn(index + reservedColumns);

	updateFrameIndexLabel(frameIndex);
	mainInterface->StartRender();
}

void cKeyframeAnimation::slotClickedNextFrame()
{
	int actualFrameIndex = ui->horizontalSlider_actualFrame->value();
	if (actualFrameIndex + 1 < keyframes->GetTotalNumberOfFrames())
	{
		ui->horizontalSlider_actualFrame->setValue(actualFrameIndex + 1);
		slotSliderMovedActualFrame(actualFrameIndex + 1);
	}
}
void cKeyframeAnimation::slotClickedPrevFrame()
{
	int actualFrameIndex = ui->horizontalSlider_actualFrame->value();
	if (actualFrameIndex - 1 >= 0)
	{
		ui->horizontalSlider_actualFrame->setValue(actualFrameIndex - 1);
		slotSliderMovedActualFrame(actualFrameIndex - 1);
	}
}

void cKeyframeAnimation::slotUpdateKeyByChart(int key, double value, int tableRow)
{
	const int parameterIndex = rowParameter.at(tableRow);
	const int vectorComponentIndex = tableRow - parameterRows.at(parameterIndex);
	cAnimationFrames::sParameterDescription parameterDescr =
		keyframes->GetListOfParameters().at(parameterIndex);

	QString fullParameterName = parameterDescr.containerName + "_" + parameterDescr.parameterName;

	cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(key);
	cOneParameter parameter = frame.parameters.GetAsOneParameter(fullParameterName);

	parameterContainer::enumVarType varType = parameterDescr.varType;

	if (varType == parameterContainer::typeVector3 || varType == parameterContainer::typeVector4)
	{
		CVector4 vect4Value = parameter.Get<CVector4>(valueActual);
		switch (vectorComponentIndex)
		{
			case 0: vect4Value.x = value; break;
			case 1: vect4Value.y = value; break;
			case 2: vect4Value.z = value; break;
			case 3: vect4Value.w = value; break;
			default: break;
		}
		parameter.Set(vect4Value, valueActual);
	}
	else if (varType == parameterContainer::typeRgb)
	{
		sRGB rgbValue = parameter.Get<sRGB>(valueActual);
		int intValue = clamp(int(value), 0, 65535);
		switch (vectorComponentIndex)
		{
			case 0: rgbValue.R = intValue; break;
			case 1: rgbValue.G = intValue; break;
			case 2: rgbValue.B = intValue; break;
			default: break;
		}
		parameter.Set(rgbValue, valueActual);
		value = intValue;
	}
	else
	{
		parameter.Set(value, valueActual);
	}

	frame.parameters.SetFromOneParameter(fullParameterName, parameter);
	keyframes->ModifyFrame(key, frame);

	int cellCollumn = key + reservedColumns;
	table->item(tableRow, cellCollumn)->setText(QString("%L1").arg(value, 0, 'g', 15));

	UpdateAnimationPathSingleParameter(tableRow);
}

void cKeyframeAnimation::slotClearKeyframe(int key, int _tableRow)
{
	const int parameterIndex = rowParameter.at(_tableRow);
	cAnimationFrames::sParameterDescription parameterDescr =
		keyframes->GetListOfParameters().at(parameterIndex);
	QString fullParameterName = parameterDescr.containerName + "_" + parameterDescr.parameterName;

	cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(key);
	cOneParameter parameter = frame.parameters.GetAsOneParameter(fullParameterName);

	parameter.SetEmpty();

	parameterContainer::enumVarType varType = parameterDescr.varType;

	int parameterFirstRow = parameterRows.at(parameterIndex);

	int cellCollumn = key + reservedColumns;

	if (varType == parameterContainer::typeVector3 || varType == parameterContainer::typeVector4
			|| varType == parameterContainer::typeRgb)
	{
		table->item(parameterFirstRow, cellCollumn)->setText("");
		table->item(parameterFirstRow + 1, cellCollumn)->setText("");
		table->item(parameterFirstRow + 2, cellCollumn)->setText("");
		if (varType == parameterContainer::typeVector4)
		{
			table->item(parameterFirstRow + 3, cellCollumn)->setText("");
		}
	}
	else
	{
		table->item(parameterFirstRow, cellCollumn)->setText("");
	}

	frame.parameters.SetFromOneParameter(fullParameterName, parameter);
	keyframes->ModifyFrame(key, frame);

	UpdateAnimationPathSingleParameter(_tableRow);
}

cKeyframeRenderThread::cKeyframeRenderThread(QString &_settingsText) : QThread()
{
	settingsText = _settingsText;
}

void cKeyframeRenderThread::startAnimationRender()
{
	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromString(settingsText);
	parSettings.Decode(gPar, gParFractal, nullptr, gKeyframes);
	gNetRender->SetAnimation(true);
	gKeyframeAnimation->RenderKeyframes(&gMainInterface->stopRequest);
	emit renderingFinished();
}

cKeyframeSaveImageThread::cKeyframeSaveImageThread(std::shared_ptr<cImage> _image,
	const QString &_filename, ImageFileSave::enumImageFileType _fileType)
{
	image = _image;
	filename = _filename;
	fileType = _fileType;
}

void cKeyframeSaveImageThread::startSaving()
{
	SaveImage(filename, fileType, image, nullptr);
	emit savingFinished();
}
