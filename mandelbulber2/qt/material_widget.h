/*
 * material_widget.h
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_WIDGET_H_
#define MANDELBULBER2_QT_MATERIAL_WIDGET_H_

#include "thumbnail_widget.h"

class cMaterialWidget: public cThumbnailWidget
{
	Q_OBJECT
public:
  cMaterialWidget(QWidget *parent);
	cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent);
  ~cMaterialWidget();
	void AssignMaterial(cParameterContainer *_params, int materialIndex, QWidget *_materialEditorWidget = NULL);
	void AssignMaterial(const QString &settings, int materialIndex);

private:
	cParameterContainer *paramsHandle;
	cParameterContainer paramsCopy;
	int actualMaterialIndex;
	QTimer *timerPeriodicRefresh;
	QTimer *timerPeriodicUpdateData;
	void Init();
	QWidget *materialEditorWidget;
	QSize sizeHint() const;
	void InitializeData();

	bool initialized;
	bool dataAssigned;
	double timeUpdateData;
	double timeAssingData;

	virtual void mousePressEvent(QMouseEvent *event);

private slots:

	void slotPeriodicRender();
	void slotPeriodicUpdateData();

public slots:
	void slotMaterialChanged();

public:
	static int previewWidth;
	static int previewHeight;

signals:
  void materialChanged(int matIndex);
  void clicked(Qt::MouseButton button);

};

#endif /* MANDELBULBER2_QT_MATERIAL_WIDGET_H_ */
