/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2023 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_QT_SETTINGS_BROWSER_H_
#define MANDELBULBER2_QT_SETTINGS_BROWSER_H_

#include <QDialog>
#include <QDateTime>
#include <QTimer>

namespace Ui
{
class cSettingsBrowser;
}

class cSettingsBrowser : public QDialog
{
	Q_OBJECT

	struct sSettingsListItem
	{
		bool isFolder = false;
		QString filename;
		QDateTime dateTime;
		QString version;
		bool hasAnimation = false;
		QString fractals;
		bool loaded = false;
	};

	enum enumOpenclMode
	{
		modeNoOpenCL = 0,
		modeOnlyMC = 1,
		modeOnlyMCHQ = 2,
		modeAll = 3,
		modeAllHQ = 4
	};

public:
	explicit cSettingsBrowser(QWidget *parent = nullptr);
	~cSettingsBrowser();
	void SetInitialFileName(const QString &_initFilename);
	QString getSelectedFileName() { return selectedFileName; }

private:
	// events
	void closeEvent(QCloseEvent *event) override;

	void CreateListOfSettings();
	void PrepareTable();
	void DeleteAllThumbnails();
	void AddRow(int rowToAdd);
	void RefreshTable();

private slots:
	void slotPressedLoad();
	void slotPressedCancel();
	void slotPressedParent();
	void slotPressedSelectDirectory();
	void slotTimer();
	void slotChangedOpenCLMode(int index);
	void slotCellDoubleClicked(int row, int column);

private:
	Ui::cSettingsBrowser *ui;

	QString actualDirectory;
	QList<sSettingsListItem> settingsList;
	int previewWidth;
	int previewHeight;
	QString selectedFileName;
	QString initFilename;

	const int previewColumnIndex = 0;
	const int fileNameColumnIndex = 1;
	const int dateColumnIndex = 2;
	const int fractalsColumnIndex = 3;
	const int effectsColumnIndex = 4;
	const int numberOfColumns = 5;

	const int refreshTimeMsec = 20;

	QTimer timer;
};

#endif /* MANDELBULBER2_QT_SETTINGS_BROWSER_H_ */
