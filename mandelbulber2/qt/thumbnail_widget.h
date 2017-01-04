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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cThumbnailWidget - promoted QWidget for displaying of fractals in an auto-rendering thumbnail
 *
 * This class holds an cImage and fractal settings can be assigned with AssignParameters().
 * The class then asynchroniously renders the fractal as a thumbnail and displays it.
 * The fractal thumbnails can also be cached in filesystem for faster loading.
 * Signals for progress and render finish can be connected, see also usage in PreviewFileDialog.
 */

#ifndef MANDELBULBER2_QT_THUMBNAIL_WIDGET_H_
#define MANDELBULBER2_QT_THUMBNAIL_WIDGET_H_

#include <QWidget>
#include <qprogressbar.h>
#include <QElapsedTimer>

// forward declarations
class cParameterContainer;
class cFractalContainer;
class cImage;

class cThumbnailWidget : public QWidget
{
	Q_OBJECT
public:
	cThumbnailWidget(QWidget *parent);
	cThumbnailWidget(int _width, int _height, int _oversample, QWidget *parent);
	~cThumbnailWidget();
	void Init(QWidget *parent);
	void SetSize(int _width, int _height, int _oversample);
	void AssignParameters(const cParameterContainer &_params, const cFractalContainer &_fractal);
	void UseOneCPUCore(bool onlyOne) { useOneCPUCore = onlyOne; }
	void DisableTimer() { disableTimer = true; }
	void DisableThumbnailCache() { disableThumbnailCache = true; }
	bool IsRendered() const { return isRendered; }
	QString GetThumbnailFileName() const;

	static int instanceCount;
	int instanceIndex;

private:
	void paintEvent(QPaintEvent *event) override;

private slots:
	void slotFullyRendered();
	void slotRandomRender();

public slots:
	void slotSetMinimumSize(int width, int height);
	void slotRender();

private:
	cImage *image;
	cParameterContainer *params;
	cFractalContainer *fractal;
	int tWidth;
	int tHeight;
	int oversample;
	QString hash;
	QString oldHash;
	QProgressBar *progressBar;
	bool stopRequest;
	bool isRendered;
	bool hasParameters;
	bool useOneCPUCore;
	bool disableTimer;
	bool disableThumbnailCache;
	// timer for random trigger for rendering (renders thumbnail even when is not visible)
	QTimer *timer;
	QElapsedTimer renderingTimeTimer;

protected:
	double lastRenderTime;

signals:
	void renderRequest();
	void thumbnailRendered();
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void settingsChanged();
};

#endif /* MANDELBULBER2_QT_THUMBNAIL_WIDGET_H_ */
