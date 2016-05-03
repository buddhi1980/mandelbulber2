/*
 * material_editor.h
 *
 *  Created on: 3 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_EDITOR_H_
#define MANDELBULBER2_QT_MATERIAL_EDITOR_H_

#include <QWidget>

namespace Ui {
	class cMaterialEditor;
}

class cMaterialEditor : public QWidget
{
	Q_OBJECT

public:
	explicit cMaterialEditor(QWidget *parent = NULL);
	~cMaterialEditor();

private:
	Ui::cMaterialEditor *ui;
};

#endif /* MANDELBULBER2_QT_MATERIAL_EDITOR_H_ */



