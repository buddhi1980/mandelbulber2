/*
 * audio_selector.cpp
 *
 *  Created on: 12 lis 2016
 *      Author: krzysztof
 */

#include <QtWidgets/QtWidgets>

#include "ui_audio_selector.h"
#include "audio_selector.h"
#include "../src/automated_widgets.hpp"
#include "../src/audio_track.h"

cAudioSelector::cAudioSelector(QWidget *parent) : QWidget(parent), ui(new Ui::cAudioSelector)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
}

cAudioSelector::~cAudioSelector()
{
	delete ui;
}

void cAudioSelector::slotLoadAudioFile()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.wav *.mp3)"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select audio file..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());

		cAudioTrack *audio = new cAudioTrack(this);
		audio->LoadAudio(filename);
	}
}

void cAudioSelector::ConnectSignals()
{
	connect(ui->pushButton_loadAudioFile, SIGNAL(clicked()), this, SLOT(slotLoadAudioFile()));
}
