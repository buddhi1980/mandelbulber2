/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * PreviewFileDialog - promoted QFileDialog for selection of fractal settings with a preview
 */

#include "preview_file_dialog.h"

#include <memory>

#include "my_progress_bar.h"
#include "thumbnail_widget.h"

#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/queue.hpp"
#include "src/radiance_hdr.h"
#include "src/render_window.hpp"
#include "src/settings.hpp"
#include "src/system_data.hpp"
#include "src/system_directories.hpp"

PreviewFileDialog::PreviewFileDialog(QWidget *parent) : QFileDialog(parent)
{
	setOption(QFileDialog::DontUseNativeDialog);

	preview = nullptr;
	vBoxLayout = new QVBoxLayout();

	checkbox = new QCheckBox(tr("Preview"), this);
	checkbox->setChecked(true);

	preview = new QLabel("", this);
	preview->setAlignment(Qt::AlignCenter);
	preview->setObjectName("label_preview");

	thumbnailSize = systemData.GetPreferredThumbnailSize() * 1.5;

	double dpiScale = devicePixelRatioF();
	thumbWidget = new cThumbnailWidget(thumbnailSize, thumbnailSize, dpiScale, this);

	description = new QLabel("", this);
	description->setAlignment(Qt::AlignCenter);
	description->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	info = new QLabel("", this);
	info->setWordWrap(true);
	info->setMaximumWidth(thumbnailSize);

	// progressBar = new MyProgressBar(this);
	// progressBar->setMaximum(1000);
	// progressBar->setAlignment(Qt::AlignCenter);
	// progressBar->hide();

	presetAddButton = new QPushButton(this);
	presetAddButton->setText(tr("Add to presets"));

	queueAddButton = new QPushButton(this);
	queueAddButton->setText(tr("Add to queue"));

	vBoxLayout->addWidget(checkbox);
	vBoxLayout->addWidget(preview);
	vBoxLayout->addWidget(thumbWidget);
	vBoxLayout->addWidget(description);
	// vBoxLayout->addWidget(progressBar);
	vBoxLayout->addWidget(info);
	vBoxLayout->addWidget(presetAddButton);
	vBoxLayout->addWidget(queueAddButton);

	thumbWidget->show();
	vBoxLayout->addStretch();

	// add to existing layout
	QGridLayout *gridLayout = static_cast<QGridLayout *>(layout());
	gridLayout->addLayout(vBoxLayout, 1, 3, 3, 1);

	adjustSize();

	connect(
		this, SIGNAL(currentChanged(const QString &)), this, SLOT(OnCurrentChanged(const QString &)));
	connect(presetAddButton, SIGNAL(clicked()), this, SLOT(OnPresetAdd()));
	connect(queueAddButton, SIGNAL(clicked()), this, SLOT(OnQueueAdd()));
	connect(thumbWidget, SIGNAL(thumbnailRendered()), this, SLOT(slotHideProgressBar()));
	connect(thumbWidget, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		this, SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
}

PreviewFileDialog::~PreviewFileDialog()
{
	// all deleted by parent widget
}

void PreviewFileDialog::OnPresetAdd() const
{
	fcopy(filename,
		systemDirectories.GetToolbarFolder() + QDir::separator() + QFileInfo(filename).fileName());
	gMainInterface->mainWindow->slotPopulateToolbar();
}

void PreviewFileDialog::OnQueueAdd() const
{
	gQueue->Append(filename, cQueue::queue_STILL);
}

void PreviewFileDialog::OnCurrentChanged(const QString &_filename)
{
	filename = _filename;
	QPixmap pixmap;
	if (QFileInfo(filename).suffix() == QString("fract") && checkbox->isChecked())
	{
		thumbWidget->show();
		description->show();
		preview->hide();
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		if (parSettings.LoadFromFile(filename))
		{
			//			progressBar->show();
			std::shared_ptr<cParameterContainer> par(new cParameterContainer);
			std::shared_ptr<cFractalContainer> parFractal(new cFractalContainer);
			InitParams(par);
			for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
				InitFractalParams(parFractal->at(i));

			InitMaterialParams(1, par);

			if (parSettings.Decode(par, parFractal))
			{
				par->Set("opencl_mode", gPar->Get<int>("opencl_mode"));
				par->Set("opencl_enabled", gPar->Get<bool>("opencl_enabled"));
				if (!gPar->Get<bool>("thumbnails_with_opencl")) par->Set("opencl_enabled", false);
				description->setText(par->Get<QString>("description"));
				thumbWidget->AssignParameters(par, parFractal);
				thumbWidget->update();
				QApplication::processEvents();
			}
			else
			{
				description->setText(" ");
				preview->setText(" ");
				info->setText(" ");
			}
		}
	}
	else
	{
		thumbWidget->hide();
		description->hide();
		preview->show();

		cRadianceHDR radianceHDR;
		int w;
		int h;
		if (radianceHDR.Init(filename, &w, &h))
		{
			radianceHDR.LoadToQPixmap(&pixmap);
		}
		else
		{
			pixmap.load(filename);
		}

		if (pixmap.isNull() || !checkbox->isChecked())
		{
			preview->setText(" ");
			info->setText(" ");
		}
		else
		{
			preview->setPixmap(
				pixmap.scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
			QString text =
				QString::number(pixmap.width()) + QString(" x ") + QString::number(pixmap.height());
			info->setText(text);
		}
	}
}

void PreviewFileDialog::slotUpdateProgressAndStatus(
	const QString &text, const QString &progressText, double progress)
{
	info->setText(text);
	//	if (!progressBar->isVisible()) progressBar->setVisible(true);
	//	progressBar->setValue(int(progress * 1000.0));
	//	progressBar->setTextVisible(true);
	//	progressBar->setFormat(progressText);
	//	update();
}

void PreviewFileDialog::slotHideProgressBar() const
{
	//	progressBar->hide();
}
