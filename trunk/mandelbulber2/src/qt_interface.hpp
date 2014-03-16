/*********************************************************
 /                   MANDELBULBER
 / Qt user interface
 /
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 / many improvements done by Rayan Hitchman
 ********************************************************/

#ifndef QT_INTERFACE_HPP_
#define QT_INTERFACE_HPP_

#include <QtGui>
#include <QtUiTools>

class InterfaceSlots: public QObject
{
Q_OBJECT
public:
	InterfaceSlots(void);

public slots:
	void testSlot();
	void load();
	void slotSliderMoved(int value);
	void slotDoubleSpinBoxChanged(double value);
	void slotLogSliderMoved(int value);
	void slotLogLineEditChanged(const QString &text);
};

class RenderedImage: public QWidget
{
Q_OBJECT

public:
	RenderedImage(QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *event);
};

struct sRGBA
{
	uchar b;
	uchar g;
	uchar r;
	uchar a;

};


#endif /* QT_INTERFACE_HPP_ */
