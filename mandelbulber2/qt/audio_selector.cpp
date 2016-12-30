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
 * cAudioSelector - This is a file selector widget for aufio files.
 */

#include <QtWidgets/QtWidgets>

#include "ui_audio_selector.h"
#include "audio_selector.h"
#include "../src/automated_widgets.hpp"
#include "../src/audio_track.h"
#include "../src/initparameters.hpp"
#include "../src/synchronize_interface.hpp"
#include "../src/animation_frames.hpp"

cAudioSelector::cAudioSelector(QWidget *parent) : QWidget(parent), ui(new Ui::cAudioSelector)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
	audio = NULL;
	animationFrames = NULL;
	setAttribute(Qt::WA_DeleteOnClose, true);
}

cAudioSelector::~cAudioSelector()
{
	SynchronizeInterfaceWindow(this, gPar, qInterface::read);
}

void cAudioSelector::slotLoadAudioFile()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Audio files (*.wav *.mp3)"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select audio file..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());

		if (animationFrames)
		{
			audio = animationFrames->GetAudioPtr(parameterName);
		}
		audio->Clear();
		audio->setFramesPerSecond(30.0);

		ui->text_animsound_soundfile->setText(filename);

		connect(audio, SIGNAL(loadingFinished()), this, SLOT(slotAudioLoaded()));
		audio->LoadAudio(filename);
	}
}

void cAudioSelector::slotAudioLoaded()
{
	audio->setFramesPerSecond(30.0); // TODO settings for frames per second
	audio->calculateFFT();					 // TODO settings for frames per second
	ui->waveForm->AssignAudioTrack(audio);
	ui->fft->AssignAudioTrack(audio);
	ui->timeRuler->SetParameters(audio, 100); // TODO hardcoded frames per keyframe
	slotFreqChanged();
	emit audioLoaded();
}

void cAudioSelector::AssignParameter(const QString &_parameterName)
{
	parameterName = _parameterName;

	setWindowTitle(tr("Set animation controlled by audio file for parameter %1").arg(parameterName));

	RenameWidget(ui->groupCheck_animsound_enable);
	RenameWidget(ui->text_animsound_soundfile);

	QList<QWidget *> listOfWidgets = ui->groupCheck_animsound_enable->findChildren<QWidget *>();

	foreach (QWidget *widget, listOfWidgets)
	{
		RenameWidget(widget);
	}

	SynchronizeInterfaceWindow(this, gPar, qInterface::write);
}

void cAudioSelector::ConnectSignals()
{
	connect(ui->pushButton_loadAudioFile, SIGNAL(clicked()), this, SLOT(slotLoadAudioFile()));
	connect(
		ui->spinbox_animsound_bandwidth, SIGNAL(valueChanged(double)), this, SLOT(slotFreqChanged()));
	connect(
		ui->spinbox_animsound_midfreq, SIGNAL(valueChanged(double)), this, SLOT(slotFreqChanged()));
	connect(
		this, SIGNAL(freqencyChanged(double, double)), ui->fft, SLOT(slotFreqChanged(double, double)));
};

void cAudioSelector::RenameWidget(QWidget *widget)
{
	QString oldName = widget->objectName();
	QString newName = oldName + "_" + parameterName;
	widget->setObjectName(newName);
}

void cAudioSelector::slotFreqChanged()
{
	if (audio)
	{
		SynchronizeInterfaceWindow(this, gPar, qInterface::read);
		double midFreq = gPar->Get<double>(FullParameterName("midfreq"));
		double bandwidth = gPar->Get<double>(FullParameterName("bandwidth"));
		audio->calculateAnimation(midFreq, bandwidth);
		ui->animAudioView->UpdateChart(audio);
		emit freqencyChanged(midFreq, bandwidth);
	}
}

QString cAudioSelector::FullParameterName(const QString &name)
{
	return QString("animsound_") + name + "_" + parameterName;
}

void cAudioSelector::AssignAnimation(cAnimationFrames *_animationFrames)
{
	animationFrames = _animationFrames;
	if (animationFrames && !parameterName.isEmpty())
	{
		audio = animationFrames->GetAudioPtr(parameterName);
		if (audio->isLoaded())
		{
			ui->waveForm->AssignAudioTrack(audio);
			ui->fft->AssignAudioTrack(audio);
			ui->timeRuler->SetParameters(audio, 100); // TODO hardcoded frames per keyframe
			slotFreqChanged();
		}
	}
}
