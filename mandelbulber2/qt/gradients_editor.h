/*
 * gradients_editor.h
 *
 *  Created on: 16 cze 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_GRADIENTS_EDITOR_H_
#define MANDELBULBER2_QT_GRADIENTS_EDITOR_H_

#include <QWidget>

namespace Ui
{
class cGradientsEditor;
}

class cGradientsEditor : public QWidget
{
	Q_OBJECT
public:
	cGradientsEditor(QWidget *parent = nullptr);
	~cGradientsEditor();

private:
	Ui::cGradientsEditor *ui;
};

#endif /* MANDELBULBER2_QT_GRADIENTS_EDITOR_H_ */
