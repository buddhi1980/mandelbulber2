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
 * cPreferencesDialog - dialog to configure program wide settings and program maintaining
 */

#ifndef MANDELBULBER2_QT_PREFERENCES_DIALOG_H_
#define MANDELBULBER2_QT_PREFERENCES_DIALOG_H_

#include <QDialog>

// forward declarations
class cAutomatedWidgets;

namespace Ui
{
class cPreferencesDialog;
}

class cPreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cPreferencesDialog(QWidget *parent = nullptr);
	~cPreferencesDialog();

private slots:
	void on_buttonBox_accepted();
	void on_pushButton_select_image_path_clicked();
	void on_pushButton_select_settings_path_clicked();
	void on_pushButton_select_textures_path_clicked();
	void on_pushButton_clear_thumbnail_cache_clicked() const;
	void on_pushButton_load_thumbnail_cache_clicked() const;
	void on_pushButton_generate_thumbnail_cache_clicked();
	void on_comboBox_ui_style_type_currentIndexChanged(int index) const;
	void on_comboBox_ui_skin_currentIndexChanged(int index) const;
	void on_pushButton_retrieve_toolbar_clicked() const;
	void on_pushButton_retrieve_materials_clicked() const;

private:
	Ui::cPreferencesDialog *ui;
	cAutomatedWidgets *automatedWidgets;
	bool initFinished;
};

#endif /* MANDELBULBER2_QT_PREFERENCES_DIALOG_H_ */
