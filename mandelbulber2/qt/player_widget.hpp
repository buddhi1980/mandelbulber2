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
 * PlayerWidget - promoted QWidget to display video of image sequence from folder
 * The folder can be assigned with SetFilePath(). The widget will play the image
 * sequence inside the folder with simple player functionality.
 */

#ifndef MANDELBULBER2_QT_PLAYER_WIDGET_HPP_
#define MANDELBULBER2_QT_PLAYER_WIDGET_HPP_

#include <QTimer>
#include <QWidget>
#include <QtWidgets>

// forward declarations
class MyDoubleSpinBox;

class PlayerWidget : public QWidget
{
	Q_OBJECT

public:
	PlayerWidget(QWidget *parent = nullptr);
	~PlayerWidget();
	void SetFilePath(QString filePath);
	QSize sizeHint() const override { return QSize(800, 600); }

public slots:
	void playPause() const;
	void stop();

private slots:
	void setPosition(int position);
	void nextFrame();
	void setFPS(double fps) const;
	void updateFromFolder();

private:
	void updateFrame();
	QAbstractButton *playPauseButton;
	QAbstractButton *stopButton;
	QSlider *positionSlider;
	QLabel *infoLabel;
	QLabel *imageLabel;
	MyDoubleSpinBox *fpsSpinBox;
	QTimer *playTimer;
	QStringList imageFiles;
	int currentIndex;
	QString dirPath;
	QFileSystemWatcher imageFolderWatcher;

protected:
	void closeEvent(QCloseEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
};

#endif /* MANDELBULBER2_QT_PLAYER_WIDGET_HPP_ */
