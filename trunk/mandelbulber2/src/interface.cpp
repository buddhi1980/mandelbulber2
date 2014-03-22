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
	mainWindow = new RenderWindow;
	mainWindow->show();

	renderedImage = new RenderedImage(mainWindow);
	mainWindow->ui->scrollAreaHLayout->addWidget(renderedImage);
	mainWindow->ui->scrollAreaWidgetContents->setMinimumSize(1000,1000);
	renderedImage->show();

	ConnectSignals();
}

void cInterface::ConnectSignals(void)
{
	QApplication::connect(mainWindow->ui->pushButton_render, SIGNAL(clicked()), mainWindow, SLOT(testSlot()));
	QApplication::connect(mainWindow->ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	ConnectSignalsForSlidersInWindow(mainWindow);
}

//Reading ad writing parameters from/to ui to/from parameters container
void cInterface::SynchronizeInterfaceWindow(QWidget *window, parameters::container *par, enumReadWrite mode)
{
	QTextStream out(stdout);
	WriteLog("SynchronizeInterfaceWindow() started");
	QList<QLineEdit *> widgetListLineEdit = window->findChildren<QLineEdit *>();

	WriteLog("SynchronizeInterfaceWindow() QLineEdit");
	foreach (QLineEdit* it, widgetListLineEdit)
	{
		out << "QLineEdit:" << it->objectName() << " Type:" << it->metaObject()->className()<< endl;

		QString name = it->objectName();
		if(name.length() > 1 && it->metaObject()->className() == QString("QLineEdit"))
		{
			QLineEdit *lineEdit = it;
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
	foreach (QDoubleSpinBox* it, widgetListDoubleSpinBox)
	{
		out << "QDoubleSpinBox:" << it->objectName() << " Type:" << it->metaObject()->className()<< endl;

		QString name = it->objectName();
		if(name.length() > 1 && it->metaObject()->className() == QString("QDoubleSpinBox"))
		{
			QDoubleSpinBox *spinbox = it;

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
	WriteLog("SynchronizeInterfaceWindow() finished");
}

//automatic setting of event slots for all sliders
void cInterface::ConnectSignalsForSlidersInWindow(QWidget *window)
{
	WriteLog("ConnectSignalsForSlidersInWindow() started");

	QList<QSlider *> widgetList = window->findChildren<QSlider *>();
	foreach (QSlider* it, widgetList)
	{
		QString name = it->objectName();
		if(name.length() > 1 && it->metaObject()->className() == QString("QSlider"))
		{
			const QSlider *slider = it;

			QString type, parameterName;
			GetNameAndType(name, &parameterName, &type);

			if(type == QString("slider"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), mainWindow, SLOT(slotSliderMoved(int)));

				QString spinBoxName = QString("spinbox_") + parameterName;
				QDoubleSpinBox *spinBox = qFindChild<QDoubleSpinBox*>(slider->parent(), spinBoxName);
				if(spinBox)
				{
					QApplication::connect(spinBox, SIGNAL(valueChanged(double)), mainWindow, SLOT(slotDoubleSpinBoxChanged(double)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinbox " << spinBoxName << " doesn't exists" << endl;
				}
			}
			if(type == QString("logslider"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), mainWindow, SLOT(slotLogSliderMoved(int)));

				QString editFieldName = QString("logedit_") + parameterName;
				QLineEdit *lineEdit = qFindChild<QLineEdit*>(slider->parent(), editFieldName);
				if(lineEdit)
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

//extract name and type string from widget name
void cInterface::GetNameAndType(QString name, QString *parameterName, QString *type)
{
	size_t firstDashPosition = name.indexOf("_");
	*type = name.left(firstDashPosition);
	*parameterName = name.mid(firstDashPosition + 1);
}
