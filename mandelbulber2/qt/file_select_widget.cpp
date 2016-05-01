/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyFileSelectLabel class - promoted QLabel widget for selecting a file
 * showing the selected file in a MyLineEdit and opening the QFileDialog by pressing the QPushButton
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 */

#include "file_select_widget.h"
#include "../src/parameters.hpp"
#include <QLabel>
#include "../src/global_data.hpp"
#include "../src/preview_file_dialog.h"
#include <algorithm>

FileSelectWidget::FileSelectWidget(QWidget *parent) : QWidget(parent)
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
	labelImage->setContentsMargins(0,0,0,0);
	labelImage->setSizePolicy(QSizePolicy::MinimumExpanding,
									 QSizePolicy::MinimumExpanding);
	labelImage->setMaximumHeight(200);
	labelImage->setMaximumWidth(400);
	labelImage->setStyleSheet("QLabel { color: red; font-weight: bold; border: 1px solid black;"
														"background-color: white; padding: 1px; }");
	QIcon iconFolder = QIcon::fromTheme("folder", QIcon(":/system/icons/folder.svg"));
	button->setIcon(iconFolder);
	layoutTextAndButton->addWidget(lineEdit);
	layoutTextAndButton->addWidget(button);
	frameTextAndButton->setLayout(layoutTextAndButton);

	layout->addWidget(frameTextAndButton);
	layout->addWidget(labelImage);

	actionResetToDefault = NULL;
	actionAddToFlightAnimation = NULL;
	actionAddToKeyframeAnimation = NULL;
	parameterContainer = NULL;
	gotDefault = false;
	defaultValue = "";
	connect(button, SIGNAL(clicked()), this, SLOT(slotSelectFile()));
	connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(slotChangedFile()));
}

FileSelectWidget::~FileSelectWidget(void)
{
}

void FileSelectWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	actionAddToFlightAnimation = menu->addAction(tr("Add to flight animation"));
	actionAddToKeyframeAnimation = menu->addAction(tr("Add to keyframe animation"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				SetPath(defaultValue);
			}
			else
			{
				qCritical() << " FileSelectWidget::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
		else if (selectedItem == actionAddToFlightAnimation)
		{
			if (parameterContainer)
			{
				gAnimFrames->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gFlightAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionAddToKeyframeAnimation)
		{
			if (parameterContainer)
			{
				gKeyframes->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gKeyframeAnimation->RefreshTable();
			}
		}
	}
	delete menu;
}

QString FileSelectWidget::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "<br>";
		toolTipText += "Default: " + defaultValue;
		setToolTip(toolTipText);
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
	QPixmap pixmap(lineEdit->text());

	if (pixmap.isNull())
	{
		labelImage->setText(QObject::tr("filepath invalid"));
	}
	else
	{
		if(pixmap.height() / pixmap.width() > 200 / 400)
		{
			labelImage->setPixmap(pixmap.scaledToHeight(200));
		}
		else
		{
			labelImage->setPixmap(pixmap.scaledToWidth(400));
		}
	}
}
