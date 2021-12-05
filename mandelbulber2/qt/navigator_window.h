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
#include "src/synchronize_interface.hpp"

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
	void AddLeftWidget(QWidget *widget);

	void SetInitialParameters(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams);
	void SetMouseClickFunction(QList<QVariant> _clickMode);
	void StartRender();
	void StopRender();
	void SynchronizeInterface(qInterface::enumReadWrite mode);

public slots:
	void slotCameraMovementModeChanged(int index);
	void slotMouseClickOnImage(int x, int y, Qt::MouseButton button) const;
	void slotMouseDragStart(int x, int y, Qt::MouseButtons buttons);
	void slotMouseDragFinish();
	void slotMouseDragDelta(int dx, int dy);
	void slotMouseWheelRotatedWithKeyOnImage(
		int x, int y, int delta, Qt::KeyboardModifiers keyModifiers);
	void slotButtonUseParameters();
	void slotButtonCancel();
	void slotSmallPartRendered(double time);
	void slotFullImageRendered();
	void slotDisablePeriodicRefresh();
	void slotReEnablePeriodicRefresh();
	void slotRefreshMainImage();

private slots:
	void slotPeriodicRefresh();
	void slotChangedComboMouseClickFunction(int index);
	void slotChangedPreviewSize();
	void slotDarkGlowEnabled(int state);

private:
	void InitPeriodicRefresh();

	Ui::cNavigatorWindow *ui;

	cManipulations *manipulations;
	QWidget *leftWidget = nullptr;

	std::shared_ptr<cParameterContainer> params;
	std::shared_ptr<cFractalContainer> fractalParams;
	std::shared_ptr<cParameterContainer> sourceParams;
	std::shared_ptr<cFractalContainer> sourceFractalParams;
	std::shared_ptr<cImage> image;

	bool stopRequest = false;

	QString autoRefreshLastHash;
	QTimer *autoRefreshTimer = nullptr;
	bool autoRefreshEnabled = false;

	QList<QVariant> mouseClickFunction;

	double lastRenderedTimeOfSmallPart = 1.0f;
	double lastSizefactor = 8.0;
	int lastIntSizeFactor = 8;
	int forcedSizeFactor = 0;

	int initImageWidth = 800;
	int initImageHeight = 600;
	double imageProportion = 1.0;
};

#endif /* MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_ */
