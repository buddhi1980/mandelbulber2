/*
 * interface.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: krzysztof
 */


#include "interface.hpp"
#include "system.hpp"
#include <QTextStream>
#include <QtUiTools/QtUiTools>

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

	WriteLog("Restoring window geometry");
	mainWindow->restoreGeometry(mainWindow->settings.value("mainWindowGeometry").toByteArray());

	WriteLog("Restoring window state");
	mainWindow->restoreState(mainWindow->settings.value("mainWindowState").toByteArray());

	WriteLog("mainWindow->show()");
	mainWindow->show();

	WriteLog("Prepare RenderedImage class");
	renderedImage = new RenderedImage(mainWindow);
	mainWindow->ui->scrollAreaHLayout->addWidget(renderedImage);
	mainWindow->ui->scrollAreaWidgetContents->setMinimumSize(1000,1000);
	renderedImage->show();

	//just for testing
	QUiLoader loader;
	QString uiFilename = systemData.sharedDir + QDir::separator() + "qt" + QDir::separator() + "fractal_mandelbulb.ui";
	QFile uiFile(uiFilename);
	uiFile.open(QFile::ReadOnly);

	QWidget *fractalWidget = loader.load(&uiFile);
	mainWindow->ui->verticalLayout_fractal_1->addWidget(fractalWidget);
	mainWindow->ui->verticalLayout_fractal_1->addStretch(1);

	fractalWidget->show();
	//end of testing

	WriteLog("ConnectSignals()");
	ConnectSignals();
}

void cInterface::ConnectSignals(void)
{
	QApplication::connect(mainWindow->ui->pushButton_render, SIGNAL(clicked()), mainWindow, SLOT(testSlot()));
	QApplication::connect(mainWindow->ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	QApplication::connect(mainWindow->ui->actionSave_docks_positions, SIGNAL(triggered()), mainWindow, SLOT(slotMenuSaveDocksPositions()));
	ConnectSignalsForSlidersInWindow(mainWindow);
	MakeColorButtonsInWindow(mainWindow);
}

//Reading ad writing parameters from/to ui to/from parameters container
void cInterface::SynchronizeInterface(parameters::container *par, parameters::container *parFractal, enumReadWrite mode)
{
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_effects, par, mode);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_image_adjustments, par, mode);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_navigation, par, mode);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_rendering_engine, par, mode);
	SynchronizeInterfaceWindow(mainWindow->ui->page_fractal_common, par, mode);
	SynchronizeInterfaceWindow(mainWindow->ui->page_fractal_hybrid, par, mode);

	SynchronizeInterfaceWindow(mainWindow->ui->tab_fractal_formula_1, &parFractal[0], mode);
	SynchronizeInterfaceWindow(mainWindow->ui->tab_fractal_formula_2, &parFractal[1], mode);
	SynchronizeInterfaceWindow(mainWindow->ui->tab_fractal_formula_3, &parFractal[2], mode);
	SynchronizeInterfaceWindow(mainWindow->ui->tab_fractal_formula_4, &parFractal[3], mode);
}

//Reading ad writing parameters from/to selected widget to/from parameters container
void cInterface::SynchronizeInterfaceWindow(QWidget *window, parameters::container *par, enumReadWrite mode)
{
	QTextStream out(stdout);
	WriteLog("SynchronizeInterfaceWindow() started");

	//----------- QLineEdit -------------------
	{
		WriteLog("SynchronizeInterfaceWindow() QLineEdit");
		QList<QLineEdit *> widgetListLineEdit = window->findChildren<QLineEdit *>();
		QList<QLineEdit *>::iterator it;

		for (it = widgetListLineEdit.begin(); it != widgetListLineEdit.end(); ++it)
		{
			out << "QLineEdit:" << (*it)->objectName() << " Type:" << (*it)->metaObject()->className() << endl;

			QString name = (*it)->objectName();
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("QLineEdit"))
			{
				QLineEdit *lineEdit = *it;
				QString text = lineEdit->text();
				out << name << " - text: " << text << endl;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);
				out << name << " - type: " << type << endl;

				//----- get vectors ------------
				if (type == QString("vect3"))
				{
					char lastChar = (parameterName.at(parameterName.length() - 1)).toAscii();
					QString nameVect = parameterName.left(parameterName.length() - 2);

					if (mode == read)
					{
						double value = lineEdit->text().toDouble();
						qDebug() << nameVect << " - " << lastChar << " axis = " << value << endl;
						CVector3 vect = par->Get<CVector3>(nameVect);

						switch (lastChar)
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
					else if (mode == write)
					{
						CVector3 vect = par->Get<CVector3>(nameVect);
						QString qtext;

						switch (lastChar)
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

				//---------- get double scalars --------
				else if (type == QString("edit") || type == QString("logedit"))
				{
					if (mode == read)
					{
						double value = lineEdit->text().toDouble();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						double value = par->Get<double>(parameterName);
						lineEdit->setText(QString::number(value));
					}
				}

				//----------- get texts ------------
				else if (type == QString("text"))
				{
					if (mode == read)
					{
						QString text = lineEdit->text();
						par->Set(parameterName, text);
					}
					else if (mode == write)
					{
						QString text = par->Get<QString>(parameterName);
						lineEdit->setText(text);
					}
				}
			}
		} //end foreach
	}

	//------------ Double spin-box --------------
	{
		WriteLog("SynchronizeInterfaceWindow() QDoubleSpinBox");
		QList<QDoubleSpinBox *> widgetListDoubleSpinBox = window->findChildren<QDoubleSpinBox*>();
		QList<QDoubleSpinBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			out << "QDoubleSpinBox:" << (*it)->objectName() << " Type:" << (*it)->metaObject()->className() << endl;
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("QDoubleSpinBox"))
			{
				QDoubleSpinBox *spinbox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (type == QString("spinbox"))
				{
					if (mode == read)
					{
						double value = spinbox->value();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						double value = par->Get<double>(parameterName);
						spinbox->setValue(value);
					}
				}
			}
		}
	}

	//------------ integer spin-box --------------
	{
		WriteLog("SynchronizeInterfaceWindow() QSpinBox");
		QList<QSpinBox *> widgetListDoubleSpinBox = window->findChildren<QSpinBox*>();
		QList<QSpinBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			out << "QDoubleSpinBox:" << (*it)->objectName() << " Type:" << (*it)->metaObject()->className() << endl;
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("QSpinBox"))
			{
				QSpinBox *spinbox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (type == QString("spinboxInt"))
				{
					if (mode == read)
					{
						int value = spinbox->value();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						int value = par->Get<int>(parameterName);
						spinbox->setValue(value);
					}
				}
			}
		}
	}

	//checkboxes
	{
		WriteLog("SynchronizeInterfaceWindow() QCheckBox");
		QList<QCheckBox *> widgetListDoubleSpinBox = window->findChildren<QCheckBox*>();
		QList<QCheckBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			out << "QCheckBox:" << (*it)->objectName() << " Type:" << (*it)->metaObject()->className() << endl;
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("QCheckBox"))
			{
				QCheckBox *checkbox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (type == QString("checkBox"))
				{
					if (mode == read)
					{
						bool value = checkbox->isChecked();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						bool value = par->Get<bool>(parameterName);
						checkbox->setChecked(value);
					}
				}
			}
		}
	}

	//groupsBox with checkbox
	{
		WriteLog("SynchronizeInterfaceWindow() QGroupBox");
		QList<QGroupBox *> widgetListDoubleSpinBox = window->findChildren<QGroupBox*>();
		QList<QGroupBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			out << "QGroupBox:" << (*it)->objectName() << " Type:" << (*it)->metaObject()->className() << endl;
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("QGroupBox"))
			{
				QGroupBox *groupbox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (type == QString("groupCheck"))
				{
					if (mode == read)
					{
						bool value = groupbox->isChecked();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						bool value = par->Get<bool>(parameterName);
						groupbox->setChecked(value);
					}
				}
			}
		}
	}

	//---------- color buttons -----------
	{
		WriteLog("SynchronizeInterfaceWindow() colorButtons");
		QList<QPushButton *> widgetListPushButton = window->findChildren<QPushButton*>();
		QList<QPushButton *>::iterator it;
		for (it = widgetListPushButton.begin(); it != widgetListPushButton.end(); ++it)
		{
			QString name = (*it)->objectName();
			out << "QDoubleSpinBox:" << (*it)->objectName() << " Type:" << (*it)->metaObject()->className() << endl;
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("QPushButton"))
			{
				QPushButton *colorButton = *it;

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
			if (type == QString("sliderInt"))
			{
				QApplication::connect(slider, SIGNAL(sliderMoved(int)), mainWindow, SLOT(slotIntSliderMoved(int)));

				QString spinboxName = QString("spinboxInt_") + parameterName;
				QSpinBox *spinbox = qFindChild<QSpinBox*>(slider->parent(), spinboxName);
				if (spinbox)
				{
					QApplication::connect(spinbox, SIGNAL(valueChanged(int)), mainWindow, SLOT(slotIntSpinBoxChanged(int)));
				}
				else
				{
					qWarning() << "ConnectSignalsForSlidersInWindow() error: spinboxInt " << spinboxName << " doesn't exists" << endl;
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

