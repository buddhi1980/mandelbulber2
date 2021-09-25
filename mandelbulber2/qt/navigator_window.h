/*
 * navigator_window.h
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_
#define MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_

#include <QDialog>
#include <memory>

namespace Ui
{
class cNavigatorWindow;
}

class cParameterContainer;
class cFractalContainer;
class cImage;

class cNavigatorWindow : public QDialog
{
	Q_OBJECT

public:
	explicit cNavigatorWindow(QWidget *parent = nullptr);
	~cNavigatorWindow();

	void SetInitialParameters(
		std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams);

private:
	Ui::cNavigatorWindow *ui;

	std::shared_ptr<cParameterContainer> params;
	std::shared_ptr<cFractalContainer> fractalParams;
	std::shared_ptr<cImage> image;
};

#endif /* MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_ */
