/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#ifndef MANDELBULBER2_QT_SETTINGS_CLEANER_H_
#define MANDELBULBER2_QT_SETTINGS_CLEANER_H_

#include <QDialog>
#include <QListWidgetItem>

#include "src/fractal_container.hpp"
#include "src/parameters.hpp"

namespace Ui
{
class cSettingsCleaner;
}

class cSettingsCleaner : public QDialog
{
	Q_OBJECT

public:
	explicit cSettingsCleaner(QWidget *parent = nullptr);
	~cSettingsCleaner();
	void runCleaner();

private:
	void UpdateProgressBar(double progress);

	// events
	void closeEvent(QCloseEvent *event) override;

private slots:
	void slotPressedStop();
	void slotPressedOK();
	void slotPressedCancel();
	void slotRemoveItem(QListWidgetItem *item);

private:
	Ui::cSettingsCleaner *ui;

	struct sDefaultedParameter
	{
		QString parameterName;
		std::shared_ptr<cParameterContainer> actualContainer;
		std::shared_ptr<cParameterContainer> originalContainer;
	};

	bool stopRequest = false;
	std::shared_ptr<cParameterContainer> actualParams;
	std::shared_ptr<cFractalContainer> actualFractalParams;
	QList<sDefaultedParameter> listOfAllDefaultedParameters;
};

#endif /* MANDELBULBER2_QT_SETTINGS_CLEANER_H_ */
