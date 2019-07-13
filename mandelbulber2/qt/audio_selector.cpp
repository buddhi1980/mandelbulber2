/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cAudioSelector - This is a file selector widget for audio files.
 */

#include "audio_selector.h"

#include <QMediaPlayer>
#include <QtWidgets/QtWidgets>

#include "ui_audio_selector.h"

#include "src/animation_frames.hpp"
#include "src/audio_track.h"
#include "src/automated_widgets.hpp"
#include "src/initparameters.hpp"
#include "src/synchronize_interface.hpp"

cAudioSelector::cAudioSelector(QWidget *parent) : QWidget(parent), ui(new Ui::cAudioSelector)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	audioOutput = nullptr;
	playStream = nullptr;

	ConnectSignals();
	animationFrames = nullptr;
	setAttribute(Qt::WA_DeleteOnClose, true);
	SetStartStopButtonsPlayingStatus(QAudio::IdleState);
}

cAudioSelector::~cAudioSelector()
{
	SynchronizeInterfaceWindow(this, gPar, qInterface::read);

	if (audioOutput)
	{
		audioOutput->stop();
		delete audioOutput;
	}

	if (playStream) delete playStream;
	delete ui;
}

void cAudioSelector::slotLoadAudioFile()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Audio files (*.wav *.mp3 *.flac *.ogg)"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select audio file..."));
	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();
		const QString filename = QDir::toNativeSeparators(filenames.first());

		if (animationFrames)
		{
			audio = animationFrames->GetAudioPtr(parameterName);
		}
		audio->Clear();
		audio->setFramesPerSecond(gPar->Get<double>("keyframe_frames_per_second"));

		ui->text_animsound_soundfile->setText(filename);

		slotPlaybackStop();
		SetStartStopButtonsPlayingStatus(QAudio::IdleState);

		connect(audio.data(), SIGNAL(loadingFinished()), this, SLOT(slotAudioLoaded()));
		audio->LoadAudio(filename);
	}
}

void cAudioSelector::slotAudioLoaded()
{
	audio->setFramesPerSecond(gPar->Get<double>("keyframe_frames_per_second"));
	audio->calculateFFT();
	ui->waveForm->AssignAudioTrack(audio);
	ui->fft->AssignAudioTrack(audio);
	ui->timeRuler->SetParameters(audio, gPar->Get<int>("frames_per_keyframe"));
	slotFreqChanged();
	audioSetup();
	SetStartStopButtonsPlayingStatus(QAudio::StoppedState);
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

void cAudioSelector::ConnectSignals() const
{
	connect(ui->pushButton_loadAudioFile, SIGNAL(clicked()), this, SLOT(slotLoadAudioFile()));
	connect(ui->pushButton_delete_audiotrack, SIGNAL(clicked()), this, SLOT(slotDeleteAudioTrack()));
	connect(
		ui->spinbox_animsound_bandwidth, SIGNAL(valueChanged(double)), this, SLOT(slotFreqChanged()));
	connect(
		ui->spinbox_animsound_midfreq, SIGNAL(valueChanged(double)), this, SLOT(slotFreqChanged()));
	connect(
		ui->checkBox_animsound_pitchmode, SIGNAL(stateChanged(int)), this, SLOT(slotFreqChanged()));
	connect(
		ui->groupCheck_animsound_decayfilter, SIGNAL(toggled(bool)), this, SLOT(slotFreqChanged()));
	connect(
		ui->groupCheck_animsound_smoothfilter, SIGNAL(toggled(bool)), this, SLOT(slotFreqChanged()));
	connect(
		ui->groupCheck_animsound_binaryfilter, SIGNAL(toggled(bool)), this, SLOT(slotFreqChanged()));
	connect(ui->spinbox_animsound_decaystrength, SIGNAL(valueChanged(double)), this,
		SLOT(slotFreqChanged()));
	connect(ui->spinbox_animsound_smoothstrength, SIGNAL(valueChanged(double)), this,
		SLOT(slotFreqChanged()));
	connect(ui->spinbox_animsound_binarythresh, SIGNAL(valueChanged(double)), this,
		SLOT(slotFreqChanged()));
	connect(ui->spinbox_animsound_binarylength, SIGNAL(valueChanged(double)), this,
		SLOT(slotFreqChanged()));
	connect(
		this, SIGNAL(frequencyChanged(double, double)), ui->fft, SLOT(slotFreqChanged(double, double)));
	connect(ui->pushButton_playback_start, SIGNAL(clicked()), this, SLOT(slotPlaybackStart()));
	connect(ui->pushButton_playback_stop, SIGNAL(clicked()), this, SLOT(slotPlaybackStop()));
	connect(this, SIGNAL(loadingProgress(QString)), ui->waveForm, SLOT(slotLoadingProgress(QString)));
	connect(
		this, SIGNAL(playPositionChanged(qint64)), ui->animAudioView, SLOT(positionChanged(qint64)));
	connect(ui->audio_position_slider, SIGNAL(sliderMoved(int)), this, SLOT(slotSeekTo(int)));
	connect(ui->fft, SIGNAL(newFrequencySelected(double, double)), this,
		SLOT(slotChangedFrequencyBand(double, double)));
}

void cAudioSelector::RenameWidget(QWidget *widget) const
{
	const QString oldName = widget->objectName();
	const QString newName = oldName + "_" + parameterName;
	widget->setObjectName(newName);
}

void cAudioSelector::slotFreqChanged()
{
	if (audio)
	{
		SynchronizeInterfaceWindow(this, gPar, qInterface::read);
		const double midFreq = gPar->Get<double>(FullParameterName("midfreq"));
		const double bandwidth = gPar->Get<double>(FullParameterName("bandwidth"));
		const bool pitchMode = gPar->Get<bool>(FullParameterName("pitchmode"));
		audio->calculateAnimation(midFreq, bandwidth, pitchMode);
		if (gPar->Get<bool>(FullParameterName("binaryfilter")))
		{
			audio->binaryFilter(gPar->Get<double>(FullParameterName("binarythresh")),
				gPar->Get<int>(FullParameterName("binarylength")));
		}
		if (gPar->Get<bool>(FullParameterName("decayfilter")))
		{
			audio->decayFilter(gPar->Get<double>(FullParameterName("decaystrength")));
		}
		if (gPar->Get<bool>(FullParameterName("smoothfilter")))
		{
			audio->smoothFilter(gPar->Get<double>(FullParameterName("smoothstrength")));
		}

		ui->animAudioView->UpdateChart(audio);
		emit frequencyChanged(midFreq, bandwidth);
	}
}

void cAudioSelector::slotPlaybackStart() const
{
	if (audio->isLoaded() && audioOutput)
	{
		switch (audioOutput->state())
		{
			case QAudio::ActiveState:
				// play -> pause
				audioOutput->suspend();
				SetStartStopButtonsPlayingStatus(QAudio::SuspendedState);
				break;
			case QAudio::SuspendedState:
				// pause -> resume
				audioOutput->resume();
				SetStartStopButtonsPlayingStatus(QAudio::ActiveState);
				break;
			case QAudio::StoppedState:
				// stopped -> play
				audioOutput->start(playStream);
				SetStartStopButtonsPlayingStatus(QAudio::ActiveState);
				break;
			case QAudio::IdleState: qWarning() << "audio not loaded yet!"; break;
			default: break;
		}
	}
}

void cAudioSelector::audioSetup()
{
	QAudioFormat format;
	format.setSampleRate(audio->getSampleRate());
	format.setChannelCount(1);
	format.setSampleSize(32);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::Float);

	if (audioOutput) delete audioOutput;
	audioOutput = new QAudioOutput(format, this);
	audioOutput->setVolume(1.0);
	audioOutput->setNotifyInterval(50);

	connect(audioOutput, SIGNAL(notify()), this, SLOT(slotPlayPositionChanged()));
	connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), this,
		SLOT(slotPlaybackStateChanged(QAudio::State)));

	playBuffer = QByteArray(
		reinterpret_cast<char *>(audio->getRawAudio()), int(audio->getLength() * sizeof(float)));

	if (playStream) delete playStream;
	playStream = new QBuffer(&playBuffer);
	playStream->open(QIODevice::ReadOnly);
	slotPlayPositionChanged();
}

void cAudioSelector::slotPlaybackStop()
{
	if (audioOutput)
	{
		audioOutput->stop();
		audioSetup();
		SetStartStopButtonsPlayingStatus(QAudio::StoppedState);
	}
}

void cAudioSelector::slotSeekTo(int position)
{
	if (playStream != nullptr)
	{
		qint64 targetPos = position * playStream->size() / ui->audio_position_slider->maximum();
		const qint64 chunkSize = 1024; // seek can only be a multiple of chunkSize
		targetPos = (targetPos / chunkSize) * chunkSize;
		playStream->seek(targetPos);
		slotPlayPositionChanged(false);
	}
}

QString cAudioSelector::FullParameterName(const QString &name) const
{
	return QString("animsound_") + name + "_" + parameterName;
}

void cAudioSelector::AssignAnimation(cAnimationFrames *_animationFrames)
{
	animationFrames = _animationFrames;
	if (animationFrames && !parameterName.isEmpty())
	{
		audio = animationFrames->GetAudioPtr(parameterName);
		connect(audio.data(), SIGNAL(loadingProgress(QString)), this, SIGNAL(loadingProgress(QString)));
		connect(audio.data(), SIGNAL(loadingFailed()), ui->waveForm, SLOT(slotLoadingFailed()));

		if (audio->isLoaded())
		{
			audio->setFramesPerSecond(gPar->Get<double>("keyframe_frames_per_second"));
			audio->calculateFFT();
			ui->waveForm->AssignAudioTrack(audio);
			ui->fft->AssignAudioTrack(audio);
			ui->timeRuler->SetParameters(audio, gPar->Get<int>("frames_per_keyframe"));
			slotFreqChanged();
			audioSetup();
			SetStartStopButtonsPlayingStatus(QAudio::StoppedState);
		}
	}
}

void cAudioSelector::slotDeleteAudioTrack()
{
	audio->Clear();
	ui->waveForm->AssignAudioTrack(audio);
	ui->fft->AssignAudioTrack(audio);
	ui->timeRuler->SetParameters(audio, gPar->Get<int>("frames_per_keyframe"));
	ui->text_animsound_soundfile->setText("");
	slotFreqChanged();
	slotPlaybackStop();
	emit audioLoaded();
}

void cAudioSelector::slotPlayPositionChanged(bool updateSlider)
{
	// set scroll indicator to current position
	const int viewOuterWidth = ui->scrollArea->width();
	const int viewInnerWidth = ui->scrollAreaWidgetContents->width();
	const double overScrollPercent = (1.0 * viewOuterWidth / viewInnerWidth) / 2.0;
	const int width = ui->scrollArea->horizontalScrollBar()->maximum();
	const double totalLengthSecs = 1.0 * audio->getLength() / audio->getSampleRate();
	const double processedSecs = 1.0 * totalLengthSecs * playStream->pos() / playStream->size();
	const double percentRuntime = processedSecs / totalLengthSecs;
	const int x = int(width * ((1.0 + overScrollPercent * 2) * percentRuntime - overScrollPercent));
	ui->scrollArea->horizontalScrollBar()->setValue(x);

	// set text of current position and slider progress
	const QString processedString =
		QDateTime::fromTime_t(uint(processedSecs)).toUTC().toString("hh:mm:ss");
	const QString totalLengthString =
		QDateTime::fromTime_t(uint(totalLengthSecs)).toUTC().toString("hh:mm:ss");
	ui->label_time->setText(QObject::tr("%1 / %2").arg(processedString, totalLengthString));
	if (updateSlider)
		ui->audio_position_slider->setValue(int(percentRuntime * ui->audio_position_slider->maximum()));

	emit playPositionChanged(int(processedSecs * 1000));
}

void cAudioSelector::slotPlaybackStateChanged(QAudio::State state) const
{
	if (state == QAudio::StoppedState || state == QAudio::IdleState)
	{
		SetStartStopButtonsPlayingStatus(QAudio::StoppedState);
	}
}

void cAudioSelector::SetStartStopButtonsPlayingStatus(QAudio::State state) const
{
	if (state == QAudio::ActiveState)
	{
		ui->pushButton_playback_start->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
		ui->pushButton_playback_start->setText(tr("Pause Audio"));
	}
	else
	{
		ui->pushButton_playback_start->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		ui->pushButton_playback_start->setText(tr("Play Audio"));
	}

	ui->pushButton_playback_start->setEnabled(state != QAudio::IdleState);
	ui->pushButton_playback_stop->setEnabled(
		state == QAudio::ActiveState || state == QAudio::SuspendedState);
}

void cAudioSelector::slotChangedFrequencyBand(double midFreq, double bandWidth) const
{
	ui->spinbox_animsound_midfreq->setValue(midFreq);
	ui->spinbox_animsound_bandwidth->setValue(bandWidth);
}
