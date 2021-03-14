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
	double lightAngleAlpha = 0.0;
	double lightAngleBeta = 0.0;

	std::shared_ptr<cParameterContainer> parameterContainer;
	cAutomatedWidgets *automatedWidgets;

private slots:
	void slotChangedLightType(int index);
	void slotChangedLightAngleX(double angle);
	void slotChangedLightAngleY(double angle);
	void slotChangedLightColor();
};

#endif /* MANDELBULBER2_QT_LIGHT_EDITOR_H_ */
