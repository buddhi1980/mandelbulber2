/*
 * image_adjustmnets_dock.h
 *
 *  Created on: 8 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_IMAGE_ADJUSTMNETS_DOCK_H_
#define MANDELBULBER2_QT_IMAGE_ADJUSTMNETS_DOCK_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"

namespace Ui
{
class cImageAdjustmentsDock;
}

class cImageAdjustmentsDock : public QWidget
{
	Q_OBJECT
public:
	explicit cImageAdjustmentsDock(QWidget *parent = NULL);
	~cImageAdjustmentsDock();
	void ConnectSignals();

	enum enumImageProportion
	{
		proportionFree = 0,
		proportion1_1 = 1,
		proportion4_3 = 2,
		proportion3_2 = 3,
		proportion16_9 = 4,
		proportion16_10 = 5,
		proportion2_1 = 6
	};

public:
	void ApplyImageChangesSetEnabled(bool enable);
	bool IsConnectDetailLevelEnabled();

public slots:
	void slotCheckedDetailLevelLock(int state);

private slots:
	void slotChangedComboPerspectiveType(int index);
	void slotPressedButtonImageApply();
	// resolution
	void slotChangedComboImageProportion(int index);
	void slotPressedResolutionPreset();
	void slotPressedImagesizeIncrease();
	void slotPressedImagesizeDecrease();
	void slotImageHeightChanged(int value);

	// Quality presets
	void slotQualityPresetVeryLow();
	void slotQualityPresetLow();
	void slotQualityPresetNormal();
	void slotQualityPresetHigh();

private:
	Ui::cImageAdjustmentsDock *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_IMAGE_ADJUSTMNETS_DOCK_H_ */
