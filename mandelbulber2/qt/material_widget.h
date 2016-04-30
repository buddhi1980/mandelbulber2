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
	void AssignMaterial(cParameterContainer *_params, int materialIndex, QWidget *_materialEditorWidget);

private:
	cParameterContainer *paramsHandle;
	int lastMaterialIndex;
	QTimer *timerPeriodicRefresh;
	void Init();
	QWidget *materialEditorWidget;

private slots:

	void slotPeriodicRender();
};

#endif /* MANDELBULBER2_QT_MATERIAL_WIDGET_H_ */
