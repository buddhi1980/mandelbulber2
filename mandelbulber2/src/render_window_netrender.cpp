/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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
 * RenderWindow class - main program window
 *
 * This file contains implementation of the netrender slots in RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#include "render_window.hpp"
#include "initparameters.hpp"
#include "netrender.hpp"
#include "synchronize_interface.hpp"

void RenderWindow::slotNetRenderServerStart()
{
	SynchronizeInterfaceWindow(ui->group_netrender, gPar, qInterface::read);
	qint32 port = gPar->Get<int>("netrender_server_local_port");
	gNetRender->SetServer(port);
}

void RenderWindow::slotNetRenderServerStop()
{
	gNetRender->DeleteServer();
}

void RenderWindow::slotNetRenderClientConnect()
{
	SynchronizeInterfaceWindow(ui->group_netrender, gPar, qInterface::read);
	QString address = gPar->Get<QString>("netrender_client_remote_address");
	qint32 port = gPar->Get<int>("netrender_client_remote_port");
	gNetRender->SetClient(address, port);
}

void RenderWindow::slotNetRenderClientDisconnect()
{
	gNetRender->DeleteClient();
}

void RenderWindow::slotNetRenderClientServerChange(int index)
{
	ui->groupBox_netrender_client_config->setVisible(index == CNetRender::netRender_CLIENT);
	ui->groupBox_netrender_server_config->setVisible(index == CNetRender::netRender_SERVER);
}

void RenderWindow::slotNetRenderClientListUpdate()
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
		header << tr("Name") << tr("Host") << tr("CPUs") << tr("Status") << tr("Lines done");
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

void RenderWindow::slotNetRenderClientListUpdate(int i)
{
	// update row i
	QTableWidget *table = ui->tableWidget_netrender_connected_clients;
	for (int j = 0; j < table->columnCount(); j++)
	{
		slotNetRenderClientListUpdate(i, j);
	}
}

void RenderWindow::slotNetRenderClientListUpdate(int i, int j)
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
			// ui->label_netrender_client_status->setStyleSheet("QLabel { background-color: " + color + ";
			// }");
			// cell->setBackgroundColor(QColor(255, 0, 0));
			break;
		}
		case 4: cell->setText(QString::number(gNetRender->GetClient(i).linesRendered)); break;
	}
}

void RenderWindow::slotNetRenderStatusServerUpdate()
{
	QString text = CNetRender::GetStatusText(gNetRender->GetStatus());
	QString color = CNetRender::GetStatusColor(gNetRender->GetStatus());
	ui->label_netrender_server_status->setText(text);
	ui->label_netrender_server_status->setStyleSheet(
		"QLabel { color: " + color + "; font-weight: bold; }");

	ui->bu_netrender_start_server->setEnabled(!gNetRender->IsServer());
	ui->bu_netrender_stop_server->setEnabled(gNetRender->IsServer());
}

void RenderWindow::slotNetRenderStatusClientUpdate()
{
	QString text = CNetRender::GetStatusText(gNetRender->GetStatus());
	QString color = CNetRender::GetStatusColor(gNetRender->GetStatus());
	ui->label_netrender_client_status->setText(text);
	ui->label_netrender_client_status->setStyleSheet(
		"QLabel { color: " + color + "; font-weight: bold; }");

	ui->bu_netrender_connect->setEnabled(!gNetRender->IsClient());
	ui->bu_netrender_disconnect->setEnabled(gNetRender->IsClient());
	ui->pushButton_render->setEnabled(!gNetRender->IsClient());
}

void RenderWindow::slotCheckBoxDisableNetRender(bool on)
{
	if (!on)
	{
		gNetRender->DeleteClient();
		gNetRender->DeleteServer();
	}
}
