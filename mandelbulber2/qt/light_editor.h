/*
 * light_editor.h
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_LIGHT_EDITOR_H_
#define MANDELBULBER2_QT_LIGHT_EDITOR_H_

#include <QWidget>

class cAutomatedWidgets;

namespace Ui
{
class cLightEditor;
}

class cLightEditor : public QWidget
{
	Q_OBJECT

public:
	explicit cLightEditor(QWidget *parent = nullptr);
	~cLightEditor() override;

	Ui::cLightEditor *ui;
	int lightIndex = -1;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_LIGHT_EDITOR_H_ */
