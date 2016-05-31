/*
 * material_selector.h
 *
 *  Created on: 28 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_SELECTOR_H_
#define MANDELBULBER2_QT_MATERIAL_SELECTOR_H_

#include <QWidget>
#include <qlabel.h>
#include <QtWidgets/QHBoxLayout>

#include "material_widget.h"

class cMaterialSelector: public QWidget
{
	Q_OBJECT;
public:
	cMaterialSelector(QWidget *parent = NULL);
	~cMaterialSelector();

	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}
	void SetMaterialIndex(int materialIndex);
	int GetMaterialIndex() {return actualValue;}

private:
	cMaterialWidget *materialWidget;
	QHBoxLayout *layout;
	QLabel *label;

	QAction *actionResetToDefault;
	QAction *actionAddToFlightAnimation;
	QAction *actionAddToKeyframeAnimation;
	cParameterContainer *parameterContainer;
	QString parameterName;

	int GetDefault();
	int defaultValue;
	bool gotDefault;
	int actualValue;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);

private slots:
	void slotClicked(Qt::MouseButton button);
	void slotMaterialSelected(int matIndex);
	void slotMaterialEdited();
};

#endif /* MANDELBULBER2_QT_MATERIAL_SELECTOR_H_ */
