/*
 * interface.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: krzysztof
 */


#include "interface.hpp"
#include "system.hpp"
#include <iostream>

cInterface *interface;

using namespace std;

cInterface::cInterface(int argc, char* argv[])
{
	printf("Hello World!\n");

	application = new QApplication(argc, argv);
	slot = new InterfaceSlots;

	QUiLoader loader;
	QDir workDirectory("/home/krzysztof/workspace/mandelbulber3/qt");
	loader.setWorkingDirectory(workDirectory);

	mainWindow = new QWidget;
	shadersWindow = new QWidget;

	{
		QFile file("/home/krzysztof/workspace/mandelbulber3/qt/render_window.ui");
		file.open(QFile::ReadOnly);
		mainWindow = loader.load(&file, NULL);
		file.close();
	}
	{
		QFile file("/home/krzysztof/workspace/mandelbulber3/qt/shaders_window.ui");
		file.open(QFile::ReadOnly);
		shadersWindow = loader.load(&file, NULL);
		file.close();
	}


	mainWindow->show();
	shadersWindow->show();

	QHBoxLayout *hboxlayoutScrolledArea = qFindChild<QHBoxLayout*>(mainWindow, "scrollAreaHLayout");
	QWidget *scrollAreaWidgetContents = qFindChild<QWidget*>(mainWindow, "scrollAreaWidgetContents");

	RenderedImage *renderedImage = new RenderedImage(mainWindow);
	hboxlayoutScrolledArea->addWidget(renderedImage);
	scrollAreaWidgetContents->setMinimumSize(1000,1000);
	renderedImage->show();

	QWidget *button = qFindChild<QPushButton*>(mainWindow, "pushButton_render");
	QApplication::connect(button, SIGNAL(clicked()), slot, SLOT(testSlot()));

	QAction *actionQuit = qFindChild<QAction*>(mainWindow, "actionQuit");
	QApplication::connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

	QAction *actionLoad = qFindChild<QAction*>(mainWindow, "actionLoad");
	QApplication::connect(actionLoad, SIGNAL(triggered()), slot, SLOT(load()));

	SetSlotsForSlidersWindow(shadersWindow);

	qimage = NULL;

}

void cInterface::SynchronizeInterfaceWindow(QWidget *window, parameters::container *par, enumReadWrite mode)
{
	WriteLog("SynchronizeInterfaceWindow() started");
	QList<QLineEdit *> widgetListLineEdit = window->findChildren<QLineEdit *>();

	WriteLog("SynchronizeInterfaceWindow() QLineEdit");
	foreach (QLineEdit* it, widgetListLineEdit)
	{
		cout << "QLineEdit:" << it->objectName().toStdString() << " Type:" << it->metaObject()->className()<< endl;

		string name = it->objectName().toStdString();
		if(name.length() > 1 && it->metaObject()->className() == string("QLineEdit"))
		{
			QLineEdit *lineEdit = it;
			string text = lineEdit->text().toStdString();
			cout << name << " - text: " << text << endl;

			string type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			//----- get vectors ------------
			if(type == string("vect3"))
			{
				char lastChar = parameterName.at(parameterName.length()-1);

				string nameVect = string(parameterName, 0, parameterName.length() - 2);

				if(mode == read)
				{
					double value = lineEdit->text().toDouble();
					cout << nameVect << " - " << lastChar << " axis = " << value << endl;
					CVector3 vect = par->Get<CVector3>(nameVect);

					switch(lastChar)
					{
						case 'x':
						vect.x = value;
						break;

						case 'y':
						vect.y = value;
						break;

						case 'z':
						vect.z = value;
						break;

						default:
						cerr << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
						break;
					}
					par->Set(nameVect, vect);
				}
				else if(mode == write)
				{
					CVector3 vect = par->Get<CVector3>(nameVect);
					QString qtext;

					switch(lastChar)
					{
						case 'x':
						qtext = QString::number(vect.x);
						break;

						case 'y':
						qtext = QString::number(vect.y);
						break;

						case 'z':
						qtext = QString::number(vect.z);
						break;

						default:
						cerr << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
						break;
					}
					lineEdit->setText(qtext);
				}
			}

			//---------- get scalars --------
			else if(type == string("edit"))
			{
				if(mode == read)
				{
					double value = lineEdit->text().toDouble();
					par->Set(parameterName, value);
				}
				else if(mode == write)
				{
					double value = par->Get<double>(parameterName);
					lineEdit->setText(QString::number(value));
				}
			}
		}
	}//end foreach

	WriteLog("SynchronizeInterfaceWindow() QDoubleSpinBox");
	QList<QDoubleSpinBox *> widgetListDoubleSpinBox = window->findChildren<QDoubleSpinBox*>();
	foreach (QDoubleSpinBox* it, widgetListDoubleSpinBox)
	{
		cout << "QDoubleSpinBox:" << it->objectName().toStdString() << " Type:" << it->metaObject()->className()<< endl;

		string name = it->objectName().toStdString();
		if(name.length() > 1 && it->metaObject()->className() == string("QDoubleSpinBox"))
		{
			QDoubleSpinBox *spinbox = it;

			string type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if(type == string("spinbox"))
			{
				if(mode == read)
				{
					double value = spinbox->value();
					par->Set(parameterName, value);
				}
				else if(mode == write)
				{
					double value = par->Get<double>(parameterName);
					spinbox->setValue(value);
				}
			}
		}
	}
	WriteLog("SynchronizeInterfaceWindow() finished");
}

void cInterface::SetSlotsForSlidersWindow(QWidget *window)
{
	WriteLog("SetSlotsForSlidersWindow() started");

	QList<QSlider *> widgetList = window->findChildren<QSlider *>();
	foreach (QSlider* it, widgetList)
	{
		string name = it->objectName().toStdString();
		if(name.length() > 1 && it->metaObject()->className() == string("QSlider"))
		{
			const QSlider *slider = it;

			string type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if(type == string("slider"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), slot, SLOT(slotSliderMoved(int)));

				string spinBoxName = string("spinbox_") + parameterName;
				QDoubleSpinBox *spinBox = qFindChild<QDoubleSpinBox*>(slider->parent(), spinBoxName.c_str());
				if(spinBox)
				{
					QApplication::connect(spinBox, SIGNAL(valueChanged(double)), slot, SLOT(slotDoubleSpinBoxChanged(double)));
				}
				else
				{
					cerr << "SetSlotsForSlidersWindow() error: spinbox " << spinBoxName << " doesn't exists" << endl;
				}
			}

		}
	}
	WriteLog("SetSlotsForSlidersWindow() finished");
}

void cInterface::GetNameAndType(string name, string *parameterName, string *type)
{
	size_t firstDashPosition = name.find("_");
	*type = string(name, 0, firstDashPosition);
	*parameterName = string(name, firstDashPosition + 1);
}
