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
 * cVoxelExportDialog class - class for handling the voxelexport dialog
 */

#ifndef MANDELBULBER2_QT_VOXEL_EXPORT_DIALOG_H_
#define MANDELBULBER2_QT_VOXEL_EXPORT_DIALOG_H_

#include <QDialog>

// forward declarations
class cAutomatedWidgets;
class cVoxelExport;

namespace Ui
{
class cVoxelExportDialog;
}

class cVoxelExportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cVoxelExportDialog(QWidget *parent = nullptr);
	~cVoxelExportDialog();

public slots:
	void on_pushButton_start_render_layers_clicked();
	void on_pushButton_stop_render_layers_clicked() const;
	void on_pushButton_select_image_path_clicked();
	void on_pushButton_show_layers_clicked();
	void slotUpdateProgressAndStatus(
		const QString &text, const QString &progressText, double progress) const;
	void slotSlicerFinished();

protected:
	void closeEvent(QCloseEvent *event) override;
	void showEvent(QShowEvent *event) override;

private:
	Ui::cVoxelExportDialog *ui;
	cAutomatedWidgets *automatedWidgets;
	bool initFinished;
	bool slicerBusy;
	cVoxelExport *voxelExport;
};

#endif /* MANDELBULBER2_QT_VOXEL_EXPORT_DIALOG_H_ */
