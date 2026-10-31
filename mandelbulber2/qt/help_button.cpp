/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Copyright (C) 2026 Mandelbulber Team
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * Implementation of the help button (see help_button.h for details).
 */

#include "help_button.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QMessageBox>
#include <QUrl>
#include <QVBoxLayout>

#include "src/files.h"
#include "src/system_directories.hpp"
#include "src/system_data.hpp"

cHelpButton::cHelpButton(QWidget *_parent) : QToolButton(_parent)
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(1, 1, 1, 1);
	setLayout(layout);
	setIcon(QIcon(":/system/icons/help-contents.svg"));

	int size = int(systemData.GetPreferredThumbnailSize() / 6.0);
	setIconSize(QSize(size - 2, size - 2));
	setMaximumSize(size, size);

	connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

cHelpButton::~cHelpButton()
{
	// nothing to destroy
}

QString cHelpButton::GetManualPath()
{
	const QString path = FilePathHelperDoc("index.html");
	if (QFile::exists(path))
	{
		return QFileInfo(path).absoluteFilePath();
	}
	return QString();
}

void cHelpButton::OpenManualSection(const QString &anchor)
{
	const QString manualPath = GetManualPath();
	if (manualPath.isEmpty())
	{
		QMessageBox::warning(nullptr, QObject::tr("Help"),
			QObject::tr("User manual not found in:\n%1").arg(systemDirectories.docDir));
		return;
	}
	QUrl url = QUrl::fromLocalFile(manualPath);
	if (!anchor.isEmpty())
	{
		url.setFragment(anchor);
	}
	QDesktopServices::openUrl(url);
}

void cHelpButton::slotClicked()
{
	// "helpAnchor" is a dynamic property assigned to the promoted widget in *.ui
	const QString anchor = property("helpAnchor").toString();
	OpenManualSection(anchor);
}

void cHelpButton::showEvent(QShowEvent *event)
{
	if (toolTip().isEmpty())
	{
		setToolTip(tr("Opens the user manual with the description of this window"));
	}
	QToolButton::showEvent(event);
}
