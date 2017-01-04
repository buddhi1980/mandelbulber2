/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cSystemTray class - class for system tray
 *
 * This class notifies the user about events and listenes for simple actions.
 */

#include "system_tray.hpp"
#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include <QMenu>
#include "../src/cimage.hpp"

cSystemTray::cSystemTray(cImage *image, QObject *parent)
{
	setParent(parent);
	this->image = image;
	isBusy = false;
	systemTrayIcon = new QSystemTrayIcon(parent);
	systemTrayIcon->setIcon(QIcon(":system/icons/mandelbulber.png"));
	QMenu *menu = new QMenu;

	QIcon renderIcon =
		QIcon::fromTheme("applications-graphics", QIcon(":system/icons/applications-graphics.svg"));
	QIcon stopIcon = QIcon::fromTheme("process-stop", QIcon(":system/icons/process-stop.svg"));
	QIcon quitIcon = QIcon::fromTheme("application-exit", QIcon(":system/icons/system-shutdown.svg"));

	stActionRender = new QAction(renderIcon, tr("Render Image"), parent);
	stActionRenderAnimation = new QAction(renderIcon, tr("Render Animation"), parent);
	stActionRenderFlight = new QAction(renderIcon, tr("Render Flight"), parent);
	stActionStop = new QAction(stopIcon, tr("Stop"), parent);
	stActionToggleNotification = new QAction(tr("Show Notifications"), parent);
	stActionQuit = new QAction(quitIcon, tr("Quit"), parent);

	stActionToggleNotification->setCheckable(true);
	stActionToggleNotification->setChecked(gPar->Get<bool>("system_tray_notify"));

	menu->addAction(stActionRender);
	menu->addAction(stActionRenderAnimation);
	menu->addAction(stActionRenderFlight);
	menu->addAction(stActionStop);
	menu->addSeparator();
	menu->addAction(stActionToggleNotification);
	menu->addSeparator();
	menu->addAction(stActionQuit);

	systemTrayIcon->setContextMenu(menu);

	connect(stActionRender, SIGNAL(triggered()), this, SLOT(slotStartRender()));
	connect(stActionStop, SIGNAL(triggered()), this, SLOT(slotStopRender()));
	connect(stActionQuit, SIGNAL(triggered()), this, SLOT(slotQuit()));
	connect(
		stActionToggleNotification, SIGNAL(toggled(bool)), this, SLOT(slotToggleNotification(bool)));
	connect(stActionRenderAnimation, SIGNAL(triggered()), this, SIGNAL(notifyRenderKeyframes()));
	connect(stActionRenderFlight, SIGNAL(triggered()), this, SIGNAL(notifyRenderFlight()));

	slotStopped();
	systemTrayIcon->show();

	// TODO remove timer and replace with proper signal connection
	// to slotStarted and slotStopped
	checkBusyTimer = new QTimer;
	checkBusyTimer->setInterval(30);
	connect(checkBusyTimer, SIGNAL(timeout()), this, SLOT(checkBusy()));
	checkBusyTimer->start();
}

cSystemTray::~cSystemTray()
{
	delete checkBusyTimer;
}

void cSystemTray::checkBusy()
{
	bool isBusyNew = image->IsUsed();
	if (isBusy != isBusyNew)
	{
		isBusy = isBusyNew;
		if (isBusy)
		{
			slotStarted();
		}
		else
		{
			slotStopped();
		}
	}
}

void cSystemTray::showMessage(QString text, QString progressText) const
{
	if (gPar->Get<bool>("system_tray_notify"))
	{
		systemTrayIcon->showMessage("Mandelbulber - " + text, progressText);
	}
}

void cSystemTray::slotStopped() const
{
	stActionStop->setEnabled(false);
	stActionRender->setEnabled(true);
	stActionRenderAnimation->setEnabled(true);
	stActionRenderFlight->setEnabled(true);
}

void cSystemTray::slotStarted() const
{
	stActionStop->setEnabled(true);
	stActionRender->setEnabled(false);
	stActionRenderAnimation->setEnabled(false);
	stActionRenderFlight->setEnabled(false);
}

void cSystemTray::slotToggleNotification(bool notify)
{
	gPar->Set("system_tray_notify", notify);
}

void cSystemTray::slotStartRender()
{
	gMainInterface->StartRender();
}

void cSystemTray::slotStopRender()
{
	gMainInterface->stopRequest = true;
}

void cSystemTray::slotQuit()
{
	gMainInterface->QuitApplicationDialog();
}
