/*
 * render_window.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: krzysztof
 */


#include "render_window.hpp"
#include "ui_render_window.h"
#include "qt_resources.res"
#include "interface.hpp"

#include <QtGui>

#define _USE_MATH_DEFINES
#include <cmath>

RenderWindow::RenderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RenderWindow)
{
    ui->setupUi(this);
}

RenderWindow::~RenderWindow()
{
    delete ui;
}


void RenderWindow::testSlot(void)
{
	using namespace std;
	qDebug() << "Object name from slot" << this->sender()->objectName() << endl;

	printf("Hello World!\n");

	mainInterface->SynchronizeInterfaceWindow(mainInterface->mainWindow, &gPar, cInterface::read);

	int width = 1000;
	int height = 1000;
	sRGBA *img = new sRGBA[width*height];
	mainInterface->qimage = new QImage((const uchar*)img, width, height, width*sizeof(sRGBA), QImage::Format_ARGB32);
	QWidget *scrollAreaWidgetContents = qFindChild<QWidget*>(mainInterface->mainWindow, "scrollAreaWidgetContents");

	for (int index = 0; index < 5; index++)
	{

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				img[x + y * width].r = (x*y+index)/3;
				img[x + y * width].g = y+index/2;
				img[x + y * width].b = x*y+index;
				img[x + y * width].a = 255;
			}

		}

		mainInterface->application->processEvents();
		mainInterface->mainWindow->update();
		scrollAreaWidgetContents->update();
	}

}

void RenderWindow::load(void)
{
	printf("load\n");
}

void RenderWindow::slotSliderMoved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinbox_") + parameterName;

	QDoubleSpinBox *spinBox = qFindChild<QDoubleSpinBox*>(this->sender()->parent(), spinBoxName);
	if(spinBox)
	{
		spinBox->setValue(value/100.0);
	}
	else
	{
		qWarning() << "slotSliderMoved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotDoubleSpinBoxChanged(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	mainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("slider_") + parameterName;

	QSlider *slider = qFindChild<QSlider*>(this->sender()->parent(), sliderName);
	if (slider)
	{
		slider->setValue(value * 100.0);
	}
	else
	{
		qWarning() << "slotDoubleSpinBoxChanged() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotLogSliderMoved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString lineEditName = QString("logedit_") + parameterName;

	QLineEdit *lineEdit = qFindChild<QLineEdit*>(this->sender()->parent(), lineEditName);
	if(lineEdit)
	{
		double dValue = pow(10.0, value/100.0);
		QString text = QString::number(dValue);
		lineEdit->setText(text);
	}
	else
	{
		qWarning() << "slotLogSliderMoved() error: lineEdit " << lineEditName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotLogLineEditChanged(const QString &text)
{
	using namespace std;
	QString lineEditName = this->sender()->objectName();
	QString type, parameterName;
	mainInterface->GetNameAndType(lineEditName, &parameterName, &type);
	QString sliderName = QString("logslider_") + parameterName;

	QSlider *slider = qFindChild<QSlider*>(this->sender()->parent(), sliderName);
	if (slider)
	{
		double value = text.toDouble();
		if(value > 0.0)
		{
			int sliderPosition = log10(text.toDouble()) * 100.0;
			slider->setValue(sliderPosition);
		}
		else
		{
			qWarning() << "slotLogLineEditChanged() error: value from " << lineEditName << " is not greater zero" << endl;
		}
	}
	else
	{
		qWarning() << "slotLogLineEditChanged() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

//=================== rendered image widget ==================/

RenderedImage::RenderedImage(QWidget *parent)
    : QWidget(parent)
{ }

void RenderedImage::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QPen pen(Qt::white, 2, Qt::SolidLine);
	painter.setPen(pen);
	painter.drawLine(0, 0, 400, 20);

	printf("paint\n");
	if(mainInterface->qimage)
	{
		printf("paintImage\n");
		painter.drawImage(QRect(0,0,1000,1000), *mainInterface->qimage, QRect(0,0,1000,1000));
	}
}

