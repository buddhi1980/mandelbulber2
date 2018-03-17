/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Widget which contains UI for rendering engine
 */

#include "dock_rendering_engine.h"

#include "ui_dock_rendering_engine.h"

#include "dock_fractal.h"
#include "dock_image_adjustments.h"
#include "dock_navigation.h"

#include "src/automated_widgets.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/netrender.hpp"
#include "src/render_window.hpp"
#include "src/synchronize_interface.hpp"

cDockRenderingEngine::cDockRenderingEngine(QWidget *parent)
		: QWidget(parent), ui(new Ui::cDockRenderingEngine)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();

	ui->groupBox_netrender_client_config->setVisible(false);
}

cDockRenderingEngine::~cDockRenderingEngine()
{
	delete ui;
}

void cDockRenderingEngine::ConnectSignals() const
{
	connect(ui->checkBox_use_default_bailout, SIGNAL(stateChanged(int)), this,
		SLOT(slotChangedCheckBoxUseDefaultBailout(int)));
	connect(ui->pushButton_optimization_LQ, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonOptimizeForLQ()));
	connect(ui->pushButton_optimization_MQ, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonOptimizeForMQ()));
	connect(ui->pushButton_optimization_HQ, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonOptimizeForHQ()));
	connect(ui->logedit_detail_level, SIGNAL(returnPressed()), this, SLOT(slotDetailLevelChanged()));
	connect(ui->comboBox_delta_DE_method, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboDistanceEstimationMethod(int)));
	connect(ui->bu_bounding_box_to_limit, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonSetBoundingBoxAsLimits()));

	// NetRender
	connect(ui->bu_netrender_connect, SIGNAL(clicked()), this, SLOT(slotNetRenderClientConnect()));
	connect(
		ui->bu_netrender_disconnect, SIGNAL(clicked()), this, SLOT(slotNetRenderClientDisconnect()));
	connect(ui->bu_netrender_start_server, SIGNAL(clicked()), this, SLOT(slotNetRenderServerStart()));
	connect(ui->bu_netrender_stop_server, SIGNAL(clicked()), this, SLOT(slotNetRenderServerStop()));
	connect(ui->comboBox_netrender_mode, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotNetRenderClientServerChange(int)));
	connect(
		ui->group_netrender, SIGNAL(toggled(bool)), this, SLOT(slotCheckBoxDisableNetRender(bool)));
	connect(ui->bu_netrender_connect, SIGNAL(clicked()), this, SLOT(slotNetRenderClientConnect()));
	connect(gNetRender, SIGNAL(NewStatusClient()), this, SLOT(slotNetRenderStatusClientUpdate()));
	connect(gNetRender, SIGNAL(NewStatusServer()), this, SLOT(slotNetRenderStatusServerUpdate()));
	connect(gNetRender, SIGNAL(ClientsChanged()), this, SLOT(slotNetRenderClientListUpdate()));
	connect(gNetRender, SIGNAL(ClientsChanged(int)), this, SLOT(slotNetRenderClientListUpdate(int)));
	connect(gNetRender, SIGNAL(ClientsChanged(int, int)), this,
		SLOT(slotNetRenderClientListUpdate(int, int)));

	connect(ui->checkBox_connect_detail_level_2, SIGNAL(stateChanged(int)), this,
		SIGNAL(stateChangedConnectDetailLevel(int)));

	connect(ui->pushButton_calculate_dist_thresh, SIGNAL(clicked()), this,
		SLOT(slotCalculateDistanceThreshold()));
}

void cDockRenderingEngine::slotNetRenderServerStart() const
{
	SynchronizeInterfaceWindow(ui->group_netrender, gPar, qInterface::read);
	qint32 port = gPar->Get<int>("netrender_server_local_port");
	gNetRender->SetServer(port);
}

void cDockRenderingEngine::slotNetRenderServerStop()
{
	gNetRender->DeleteServer();
}

void cDockRenderingEngine::slotNetRenderClientConnect() const
{
	SynchronizeInterfaceWindow(ui->group_netrender, gPar, qInterface::read);
	QString address = gPar->Get<QString>("netrender_client_remote_address");
	qint32 port = gPar->Get<int>("netrender_client_remote_port");
	gNetRender->SetClient(address, port);
}

void cDockRenderingEngine::slotNetRenderClientDisconnect()
{
	gNetRender->DeleteClient();
}

void cDockRenderingEngine::slotNetRenderClientServerChange(int index) const
{
	ui->groupBox_netrender_client_config->setVisible(index == CNetRender::netRender_CLIENT);
	ui->groupBox_netrender_server_config->setVisible(index == CNetRender::netRender_SERVER);
}

void cDockRenderingEngine::slotNetRenderClientListUpdate() const
{
	QTableWidget *table = ui->tableWidget_netrender_connected_clients;

	// reset table
	if (gNetRender->GetClientCount() == 0)
	{
		table->clear();
		return;
	}

	// init table
	if (table->columnCount() == 0)
	{
		QStringList header;
		header << tr("Name") << tr("Host") << tr("CPUs") << tr("Status") << tr("Lines done")
					 << tr("Actions");
		table->setColumnCount(header.size());
		table->setHorizontalHeaderLabels(header);
	}

	// change table
	if (table->rowCount() != gNetRender->GetClientCount())
	{
		table->setRowCount(gNetRender->GetClientCount());
	}

	// update table
	for (int i = 0; i < table->rowCount(); i++)
	{
		slotNetRenderClientListUpdate(i);
	}
}

void cDockRenderingEngine::slotNetRenderClientListUpdate(int i) const
{
	// update row i
	QTableWidget *table = ui->tableWidget_netrender_connected_clients;
	for (int j = 0; j < table->columnCount(); j++)
	{
		slotNetRenderClientListUpdate(i, j);
	}
}

void cDockRenderingEngine::slotNetRenderClientListUpdate(int i, int j) const
{
	// update element in row i, column j
	QTableWidget *table = ui->tableWidget_netrender_connected_clients;

	QTableWidgetItem *cell = table->item(i, j);
	if (!cell)
	{
		cell = new QTableWidgetItem;
		table->setItem(i, j, cell);
	}

	switch (j)
	{
		case 0: cell->setText(gNetRender->GetClient(i).name); break;
		case 1: cell->setText(gNetRender->GetClient(i).socket->peerAddress().toString()); break;
		case 2: cell->setText(QString::number(gNetRender->GetClient(i).clientWorkerCount)); break;
		case 3:
		{
			QString text = CNetRender::GetStatusText(gNetRender->GetClient(i).status);
			QString color = CNetRender::GetStatusColor(gNetRender->GetClient(i).status);

			cell->setText(text);
			cell->setTextColor(color);
			cell->setBackgroundColor(Qt::white);
			break;
		}
		case 4: cell->setText(QString::number(gNetRender->GetClient(i).linesRendered)); break;
		case 5:
		{
			QFrame *frame = new QFrame;
			QGridLayout *gridLayout = new QGridLayout;
			QToolButton *actionKickAndKill = new QToolButton;
			actionKickAndKill->setIcon(actionKickAndKill->style()->standardIcon(QStyle::SP_TrashIcon));
			actionKickAndKill->setFixedSize(24, 24);
			actionKickAndKill->setObjectName(QString::number(i));
			gridLayout->setContentsMargins(2, 2, 2, 2);
			QObject::connect(
				actionKickAndKill, SIGNAL(clicked()), this, SLOT(slotNetRenderKickAndKill()));
			gridLayout->addWidget(actionKickAndKill, 0, 0);
			gridLayout->setSpacing(0);
			frame->setLayout(gridLayout);
			table->setCellWidget(i, j, frame);
			break;
		}
		default: break;
	}
}

void cDockRenderingEngine::slotNetRenderStatusServerUpdate() const
{
	QString text = CNetRender::GetStatusText(gNetRender->GetStatus());
	QString color = CNetRender::GetStatusColor(gNetRender->GetStatus());
	ui->label_netrender_server_status->setText(text);
	ui->label_netrender_server_status->setStyleSheet(
				"QLabel { color: " + color + "; font-weight: bold; border: 2px solid darkgray; border-radius: 3px;"
						"background: white; text-align: center; qproperty-alignment: AlignCenter;}");

	ui->bu_netrender_start_server->setEnabled(!gNetRender->IsServer());
	ui->bu_netrender_stop_server->setEnabled(gNetRender->IsServer());
}

void cDockRenderingEngine::slotNetRenderStatusClientUpdate() const
{
	QString text = CNetRender::GetStatusText(gNetRender->GetStatus());
	QString color = CNetRender::GetStatusColor(gNetRender->GetStatus());
	ui->label_netrender_client_connected_server->setText(gNetRender->GetServerName());
	ui->label_netrender_client_status->setText(text);
	ui->label_netrender_client_status->setStyleSheet(
		"QLabel { color: " + color + "; font-weight: bold; border: 2px solid darkgray; border-radius: 3px;"
				"background: white; text-align: center; qproperty-alignment: AlignCenter;}");

	ui->bu_netrender_connect->setEnabled(!gNetRender->IsClient());
	ui->bu_netrender_disconnect->setEnabled(gNetRender->IsClient());
	gMainInterface->mainWindow->GetWidgetDockNavigation()->setEnabled(!gNetRender->IsClient());
}

void cDockRenderingEngine::slotCheckBoxDisableNetRender(bool on)
{
	if (!on)
	{
		gNetRender->DeleteClient();
		gNetRender->DeleteServer();
	}
}

void cDockRenderingEngine::slotNetRenderKickAndKill()
{
	QString buttonName = this->sender()->objectName();
	gNetRender->KickAndKillClient(buttonName.toInt());
}

void cDockRenderingEngine::SynchronizeInterfaceDistanceEstimation(cParameterContainer *par) const
{
	SynchronizeInterfaceWindow(ui->groupBox_detailLevel, par, qInterface::write);
}

void cDockRenderingEngine::ComboDeltaDEFunctionSetEnabled(bool enabled) const
{
	ui->comboBox_delta_DE_function->setEnabled(enabled);
}

int cDockRenderingEngine::ComboDeltaDEMethodCurrentIndex() const
{
	return ui->comboBox_delta_DE_method->currentIndex();
}

void cDockRenderingEngine::slotChangedComboDistanceEstimationMethod(int index) const
{
	ui->comboBox_delta_DE_function->setEnabled(
		gMainInterface->mainWindow->GetWidgetDockFractal()->AreHybridFractalsEnabled()
		|| index == int(fractal::forceDeltaDEMethod));
}

void cDockRenderingEngine::CheckboxConnectDetailLevelSetCheckState(Qt::CheckState state) const
{
	ui->checkBox_connect_detail_level_2->setCheckState(state);
}

void cDockRenderingEngine::UpdateLabelWrongDEPercentage(const QString &text) const
{
	ui->label_wrong_DE_percentage->setText(text);
}

void cDockRenderingEngine::UpdateLabelUsedDistanceEstimation(const QString &text) const
{
	ui->label_used_distance_estimation->setText(text);
}

void cDockRenderingEngine::slotChangedCheckBoxUseDefaultBailout(int state) const
{
	ui->logedit_bailout->setEnabled(!state);
}

void cDockRenderingEngine::slotDetailLevelChanged()
{
	if (gMainInterface->mainWindow->GetWidgetDockImageAdjustments()->IsConnectDetailLevelEnabled())
	{
		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
		gMainInterface->lockedDetailLevel = gPar->Get<double>("detail_level");
		gMainInterface->lockedImageResolution =
			CVector2<int>(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	}
}

void cDockRenderingEngine::slotPressedButtonOptimizeForLQ()
{
	gMainInterface->OptimizeStepFactor(1.0);
}

void cDockRenderingEngine::slotPressedButtonOptimizeForMQ()
{
	gMainInterface->OptimizeStepFactor(0.1);
}
void cDockRenderingEngine::slotPressedButtonOptimizeForHQ()
{
	gMainInterface->OptimizeStepFactor(0.01);
}
void cDockRenderingEngine::slotPressedButtonSetBoundingBoxAsLimits()
{
	gMainInterface->SetBoundingBoxAsLimits();
}

void cDockRenderingEngine::slotCalculateDistanceThreshold()
{
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
	double distance = gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"));
	double detailLevel = gPar->Get<double>("detail_level");
	double imageWidth = gPar->Get<int>("image_width");
	double fov = gPar->Get<int>("fov");
	double distThresh = fov / detailLevel / imageWidth * distance * 0.5;
	gPar->Set("DE_thresh", distThresh);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
}
