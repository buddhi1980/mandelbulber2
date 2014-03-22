/*
 * interface.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: krzysztof
 */


#include "interface.hpp"
#include "system.hpp"
#include <QTextStream>

cInterface *mainInterface;

//constructor of interface (loading of ui files)
cInterface::cInterface()
{
	mainWindow = NULL;
	application = NULL;
	qimage = NULL;
	renderedImage = NULL;
}

void cInterface::ShowUi(void)
{
	WriteLog("Prepare RenderWindow class");
	mainWindow = new RenderWindow;

	WriteLog("mainWindow->show()");
	mainWindow->show();

	WriteLog("Prepare RenderedImage class");
	renderedImage = new RenderedImage(mainWindow);
	mainWindow->ui->scrollAreaHLayout->addWidget(renderedImage);
	mainWindow->ui->scrollAreaWidgetContents->setMinimumSize(1000,1000);
	renderedImage->show();



	WriteLog("ConnectSignals()");
	ConnectSignals();
}

void cInterface::ConnectSignals(void)
{
	QApplication::connect(mainWindow->ui->pushButton_render, SIGNAL(clicked()), mainWindow, SLOT(testSlot()));
	QApplication::connect(mainWindow->ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	ConnectSignalsForSlidersInWindow(mainWindow);
	MakeColorButtonsInWindow(mainWindow);
}

//Reading ad writing parameters from/to ui to/from parameters container
void cInterface::SynchronizeInterfaceWindow(QWidget *window, parameters::container *par, enumReadWrite mode)
{
	QTextStream out(stdout);
	WriteLog("SynchronizeInterfaceWindow() started");
	WriteLog("SynchronizeInterfaceWindow() QLineEdit");
	QList<QLineEdit *> widgetListLineEdit = window->findChildren<QLineEdit *>();
	QList<QLineEdit *>::iterator it1;

	for (it1 = widgetListLineEdit.begin(); it1 != widgetListLineEdit.end(); ++it1)
	{
		out << "QLineEdit:" << (*it1)->objectName() << " Type:" << (*it1)->metaObject()->className()<< endl;

		QString name = (*it1)->objectName();
		if(name.length() > 1 && (*it1)->metaObject()->className() == QString("QLineEdit"))
		{
			QLineEdit *lineEdit = *it1;
			QString text = lineEdit->text();
			out << name << " - text: " << text << endl;

			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			//----- get vectors ------------
			if(type == QString("vect3"))
			{
				char lastChar = (parameterName.at(parameterName.length()-1)).toAscii();
				QString nameVect = parameterName.left(parameterName.length() - 2);

				if(mode == read)
				{
					double value = lineEdit->text().toDouble();
					qDebug() << nameVect << " - " << lastChar << " axis = " << value << endl;
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
						qWarning() << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
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
						qWarning() << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
						break;
					}
					lineEdit->setText(qtext);
				}
			}

			//---------- get scalars --------
			else if(type == QString("edit") || type == QString("logedit"))
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
	QList<QDoubleSpinBox *>::iterator it2;
	for (it2 = widgetListDoubleSpinBox.begin(); it2 != widgetListDoubleSpinBox.end(); ++it2)
	{
		QString name = (*it2)->objectName();
		out << "QDoubleSpinBox:" << (*it2)->objectName() << " Type:" << (*it2)->metaObject()->className()<< endl;
		if(name.length() > 1 && (*it2)->metaObject()->className() == QString("QDoubleSpinBox"))
		{
			QDoubleSpinBox *spinbox = *it2;

			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if(type == QString("spinbox"))
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

	WriteLog("SynchronizeInterfaceWindow() colorButtons");
	QList<QPushButton *> widgetListPushButton = window->findChildren<QPushButton*>();
	QList<QPushButton *>::iterator it3;
	for (it3 = widgetListPushButton.begin(); it3 != widgetListPushButton.end(); ++it3)
	{
		QString name = (*it3)->objectName();
		out << "QDoubleSpinBox:" << (*it3)->objectName() << " Type:" << (*it3)->metaObject()->className() << endl;
		if (name.length() > 1 && (*it3)->metaObject()->className() == QString("QPushButton"))
		{
			QPushButton *colorButton = *it3;

			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if (type == QString("colorButton"))
			{
				if (mode == read)
				{
					sRGB color;
					color.R = colorButton->property("selectedColor_r").toInt();
					color.G = colorButton->property("selectedColor_g").toInt();
					color.B = colorButton->property("selectedColor_b").toInt();
					par->Set(parameterName, color);
				}
				else if (mode == write)
				{
					sRGB color = par->Get<sRGB>(parameterName);
					QColor qcolor(color.R / 256, color.G / 256, color.B / 256);
					MakeIconForButton(qcolor, colorButton);
					colorButton->setProperty("selectedColor_r", color.R);
					colorButton->setProperty("selectedColor_g", color.G);
					colorButton->setProperty("selectedColor_b", color.B);
				}
			}
		}
	}
	WriteLog("SynchronizeInterfaceWindow() finished");
}

//automatic setting of event slots for all sliders
void cInterface::ConnectSignalsForSlidersInWindow(QWidget *window)
{
	WriteLog("ConnectSignalsForSlidersInWindow() started");

	QList<QSlider *> widgetList = window->findChildren<QSlider *>();
	QList<QSlider *>::iterator it;
	for (it = widgetList.begin(); it != widgetList.end(); ++it)
	{
		QString name = (*it)->objectName();
		if (name.length() > 1 && (*it)->metaObject()->className() == QString("QSlider"))
		{
			const QSlider *slider = *it;

			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if (type == QString("slider"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), mainWindow, SLOT(slotSliderMoved(int)));

				QString spinBoxName = QString("spinbox_") + parameterName;
				QDoubleSpinBox *spinBox = qFindChild<QDoubleSpinBox*>(slider->parent(), spinBoxName);
				if (spinBox)
				{
					QApplication::connect(spinBox, SIGNAL(valueChanged(double)), mainWindow, SLOT(slotDoubleSpinBoxChanged(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinbox " << spinBoxName << " doesn't exists" << endl;
				}
			}
			if (type == QString("logslider"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), mainWindow, SLOT(slotLogSliderMoved(int)));

				QString editFieldName = QString("logedit_") + parameterName;
				QLineEdit *lineEdit = qFindChild<QLineEdit*>(slider->parent(), editFieldName);
				if (lineEdit)
				{
					QApplication::connect(lineEdit, SIGNAL(textChanged(const QString&)), mainWindow, SLOT(slotLogLineEditChanged(const QString&)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: lineEdit " << editFieldName << " doesn't exists" << endl;
				}
			}

		}
	}
	WriteLog("ConnectSignalsForSlidersInWindow() finished");
}

//automatic setting of event slots for all colorButtons and preparation of buttons
void cInterface::MakeColorButtonsInWindow(QWidget *window)
{
	WriteLog("MakeColorButtonsInWindow() started");

	QList<QPushButton *> widgetList = window->findChildren<QPushButton *>();
	QList<QPushButton *>::iterator it;
	for(it = widgetList.begin(); it != widgetList.end(); ++it)
	{
		QString name = (*it)->objectName();
		if(name.length() > 1 && (*it)->metaObject()->className() == QString("QPushButton"))
		{
			QPushButton *pushButton = (*it);
			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if(type == QString("colorButton"))
			{
				pushButton->setText("");
				QColor color(255,255,255);
				MakeIconForButton(color, pushButton);
				QApplication::connect(pushButton, SIGNAL(clicked()), mainWindow, SLOT(slotPresedOnColorButton()));
			}
		}
	}
	WriteLog("MakeColorButtonsInWindow() finished");
}

//extract name and type string from widget name
void cInterface::GetNameAndType(QString name, QString *parameterName, QString *type)
{
	size_t firstDashPosition = name.indexOf("_");
	*type = name.left(firstDashPosition);
	*parameterName = name.mid(firstDashPosition + 1);
}

//function to create icons with actual color in ColorButtons
void MakeIconForButton(QColor &color, QPushButton *pushbutton)
{
	const int w = 40;
	const int h = 15;
	QPixmap pix(w,h);
	QPainter painter(&pix);
	painter.fillRect(QRect(0,0,w,h), color);
	painter.drawRect(0,0,w-1,h-1);
	QIcon icon(pix);
	pushbutton->setIcon(icon);
	pushbutton->setIconSize(QSize(w,h));
}

