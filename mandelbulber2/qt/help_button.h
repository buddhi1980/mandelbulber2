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
 * Help button widget - opens the html user manual (doc/index.html),
 * optionally scrolled to a given section anchor. The anchor is provided
 * by the "helpAnchor" dynamic property set in the *.ui file.
 */

#ifndef MANDELBULBER2_QT_HELP_BUTTON_H_
#define MANDELBULBER2_QT_HELP_BUTTON_H_

#include <QToolButton>

class cHelpButton : public QToolButton
{
	Q_OBJECT

public:
	cHelpButton(QWidget *parent = nullptr);
	~cHelpButton() override;

	// absolute path to doc/index.html; empty when the manual is not available
	static QString GetManualPath();

	// opens the manual in the system browser; anchor may be empty
	static void OpenManualSection(const QString &anchor);

protected:
	void showEvent(QShowEvent *event) override;

private slots:
	void slotClicked();
};

#endif /* MANDELBULBER2_QT_HELP_BUTTON_H_ */
