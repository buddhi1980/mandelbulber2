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
 * MyFileSelectLabel class - promoted QLabel widget for selecting a file
 * showing the selected file in a MyLineEdit and opening the QFileDialog by pressing the QPushButton
 */

#include "file_select_widget.h"
#include "../src/animation_flight.hpp"
#include "preview_file_dialog.h"

FileSelectWidget::FileSelectWidget(QWidget *parent) : QWidget(parent), CommonMyWidgetWrapper(this)
{
	QFrame *frameTextAndButton = new QFrame;
	QHBoxLayout *layoutTextAndButton = new QHBoxLayout;
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);
	layoutTextAndButton->setContentsMargins(0, 0, 0, 0);
	layoutTextAndButton->setSpacing(2);
	lineEdit = new QLineEdit;
	button = new QPushButton;
	labelImage = new QLabel;
	labelImage->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	labelImage->setContentsMargins(0, 0, 0, 0);
	labelImage->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	labelImage->setMaximumHeight(150);
	labelImage->setMaximumWidth(300);
	labelImage->setStyleSheet(
		"QLabel { color: red; font-weight: bold; border: 1px solid black;"
		"background-color: white; padding: 1px; }");
	QIcon iconFolder = QIcon::fromTheme("folder", QIcon(":/system/icons/folder.svg"));
	button->setIcon(iconFolder);
	layoutTextAndButton->addWidget(lineEdit);
	layoutTextAndButton->addWidget(button);
	frameTextAndButton->setLayout(layoutTextAndButton);

	layout->addWidget(frameTextAndButton);
	layout->addWidget(labelImage);

	defaultValue = "";
	connect(button, SIGNAL(clicked()), this, SLOT(slotSelectFile()));
	connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(slotChangedFile()));
}

FileSelectWidget::~FileSelectWidget()
{
}

void FileSelectWidget::resetToDefault()
{
	SetPath(defaultValue);
}

QString FileSelectWidget::getDefaultAsString()
{
	return defaultValue;
}

QString FileSelectWidget::getFullParameterName()
{
	return parameterName;
}

void FileSelectWidget::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event);
}

void FileSelectWidget::paintEvent(QPaintEvent *event)
{
	QFont f = font();
	f.setBold(lineEdit->text() != GetDefault());
	QWidget::paintEvent(event);
}

QString FileSelectWidget::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		QString val = parameterContainer->GetDefault<QString>(parameterName);
		defaultValue = val;
		gotDefault = true;
		setToolTipText();
	}
	return defaultValue;
}

void FileSelectWidget::slotSelectFile()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(QDir::toNativeSeparators(lineEdit->text()));
	dialog.selectFile(QDir::toNativeSeparators(lineEdit->text()));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select file for %1").arg(objectName()));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		SetPath(filename);
	}
}

void FileSelectWidget::SetPath(QString path)
{
	lineEdit->setText(path);
	slotChangedFile();
}

void FileSelectWidget::slotChangedFile()
{
	if (lineEdit->text() != actualText)
	{
		actualText = lineEdit->text();
		QPixmap pixmap(lineEdit->text());

		if (pixmap.isNull())
		{
			labelImage->setText(QObject::tr("filepath invalid"));
		}
		else
		{
			if (pixmap.height() / pixmap.width() > 150 / 300)
			{
				labelImage->setPixmap(pixmap.scaledToHeight(150, Qt::SmoothTransformation));
			}
			else
			{
				labelImage->setPixmap(pixmap.scaledToWidth(300, Qt::SmoothTransformation));
			}
		}
	}
}
