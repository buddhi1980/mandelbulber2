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

#include <stdio.h>
#include "qt_interface.hpp"
#include <iostream>
#include "qt_interface.moc.hpp"
#include "interface.hpp"
#include "parameters.hpp"

InterfaceSlots::InterfaceSlots(void)
{
}

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
	if(interface->qimage)
	{
		printf("paintImage\n");
		painter.drawImage(QRect(0,0,1000,1000), *interface->qimage, QRect(0,0,1000,1000));
	}
}

void InterfaceSlots::testSlot(void)
{
	using namespace std;
	cout << "Object name from slot" << this->sender()->objectName().toStdString() << endl;

	printf("Hello World!\n");

	interface->ReadInterfaceWindow(interface->mainWindow, &gPar);

	int width = 1000;
	int height = 1000;
	sRGBA *img = new sRGBA[width*height];
	interface->qimage = new QImage((const uchar*)img, width, height, width*sizeof(sRGBA), QImage::Format_ARGB32);
	QWidget *scrollAreaWidgetContents = qFindChild<QWidget*>(interface->mainWindow, "scrollAreaWidgetContents");

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

		interface->application->processEvents();
		interface->mainWindow->update();
		scrollAreaWidgetContents->update();
	}

}

void InterfaceSlots::load(void)
{
	printf("load\n");
}

void InterfaceSlots::slotSliderMoved(int value)
{
	using namespace std;
	string sliderName = this->sender()->objectName().toStdString();
	string type, parameterName;
	interface->GetNameAndType(sliderName, &parameterName, &type);
	string spinBoxName = string("spinbox_") + parameterName;

	QDoubleSpinBox *spinBox = qFindChild<QDoubleSpinBox*>(this->sender()->parent(), spinBoxName.c_str());
	if(spinBox)
	{
		spinBox->setValue(value/100.0);
	}
	else
	{
		cerr << "slotSliderMoved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void InterfaceSlots::slotDoubleSpinBoxChanged(double value)
{
	using namespace std;
	string spinBoxName = this->sender()->objectName().toStdString();
	string type, parameterName;
	interface->GetNameAndType(spinBoxName, &parameterName, &type);
	string sliderName = string("slider_") + parameterName;

	QSlider *slider = qFindChild<QSlider*>(this->sender()->parent(), sliderName.c_str());
	if (slider)
	{
		slider->setValue(value * 100.0);
	}
	else
	{
		cerr << "slotDoubleSpinBoxChanged() error: slider " << sliderName << " doesn't exists" << endl;
	}
}


