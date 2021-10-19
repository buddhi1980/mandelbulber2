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
#include <QString>
#include <QList>
#include <QVariant>

namespace Ui
{
class cNavigatorWindow;
}

class cParameterContainer;
class cFractalContainer;
class cImage;
class cManipulations;

class cNavigatorWindow : public QDialog
{
	Q_OBJECT

public:
	explicit cNavigatorWindow(QWidget *parent = nullptr);
	~cNavigatorWindow();

	void SetInitialParameters(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams);
	void SetMouseClickFunction(QList<QVariant> _clickMode) { mouseClickFunction = _clickMode; }
	void StartRender();
	void StopRender();

public slots:
	void slotCameraMovementModeChanged(int index);
	void slotMouseClickOnImage(int x, int y, Qt::MouseButton button) const;
	void slotMouseDragStart(int x, int y, Qt::MouseButtons buttons);
	void slotMouseDragFinish();
	void slotMouseDragDelta(int dx, int dy);

private:
	Ui::cNavigatorWindow *ui;

	cManipulations *manipulations;

	std::shared_ptr<cParameterContainer> params;
	std::shared_ptr<cFractalContainer> fractalParams;
	std::shared_ptr<cImage> image;

	bool stopRequest = false;
	QString autoRefreshLastHash;

	QList<QVariant> mouseClickFunction;
};

#endif /* MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_ */
