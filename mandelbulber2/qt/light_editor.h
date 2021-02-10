/*
 * light_editor.h
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_LIGHT_EDITOR_H_
#define MANDELBULBER2_QT_LIGHT_EDITOR_H_

#include <QWidget>
#include <memory>

class cAutomatedWidgets;
class cParameterContainer;

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
	void AssignLight(std::shared_ptr<cParameterContainer> params, int index);

private:
	Ui::cLightEditor *ui;
	int lightIndex = -1;

	bool isLightAssigned = false;

	std::shared_ptr<cParameterContainer> parameterContainer;
	cAutomatedWidgets *automatedWidgets;

private slots:
	void slotChangedLightType(int index);
};

#endif /* MANDELBULBER2_QT_LIGHT_EDITOR_H_ */
