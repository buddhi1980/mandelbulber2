/*
 * randomizer_dialog.cpp
 *
 *  Created on: 29 gru 2019
 *      Author: krzysztof
 */

#include "randomizer_dialog.h"

#include "ui_randomizer_dialog.h"

#include "common_my_widget_wrapper.h"
#include "my_group_box.h"
#include "src/initparameters.hpp"
#include "src/fractal_container.hpp"

cRandomizerDialog::cRandomizerDialog(QWidget *parent)
		: QDialog(parent), ui(new Ui::cRandomizerDialog)
{
	ui->setupUi(this);

	randomizer.Initialize(QTime::currentTime().msec());

	connect(ui->pushButton_heavy, &QPushButton::clicked, this,
		&cRandomizerDialog::slotClieckedHeavyRandomize);
	connect(ui->pushButton_medium, &QPushButton::clicked, this,
		&cRandomizerDialog::slotClieckedMediumRandomize);
	connect(ui->pushButton_slight, &QPushButton::clicked, this,
		&cRandomizerDialog::slotClieckedSlightRandomize);
}

cRandomizerDialog::~cRandomizerDialog()
{
	delete ui;
}

void cRandomizerDialog::Randomize(enimRandomizeStrength strength)
{
	// local copy of parameters
	cParameterContainer params = *gPar;
	cFractalContainer fractParams = *gParFractal;

	// initialize parameter containers
	listOfVersions.clear();
	for (int i = 0; i < numberOfVersions; i++)
	{
		sParameterVersion version;
		version.params = params;
		version.fractParams = fractParams;
		listOfVersions.append(version);
	}

	for (int i = 0; i < numberOfVersions; i++)
	{
		qDebug() << "Version " << i;

		RandomizeParameters(strength, &listOfVersions[i].params, &listOfVersions[i].fractParams);

		QString widgetName = QString("previewwidget_%1").arg(i + 1, 2, 10, QChar('0'));
		qDebug() << widgetName;
		cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
		qDebug() << previewWidget;
		previewWidget->SetSize(192, 128, 2);
		previewWidget->AssignParameters(listOfVersions.at(i).params, listOfVersions.at(i).fractParams);
		previewWidget->update();
	}
}

void cRandomizerDialog::slotClieckedSlightRandomize()
{
	Randomize(randomizeSlight);
}

void cRandomizerDialog::slotClieckedMediumRandomize()
{
	Randomize(randomizeMedium);
}

void cRandomizerDialog::AssignSourceWidget(const QWidget *sourceWidget)
{
	int level = 0;
	CreateParametersTreeInWidget(&parametersTree, sourceWidget, level, 0);
}

void cRandomizerDialog::RandomizeParameters(
	enimRandomizeStrength strength, cParameterContainer *params, cFractalContainer *fractal)
{
	int numberOfParameters = parametersTree.GetSize();
	int numberOfParametersToChange = 0;
	double randomScale = 1.0;

	switch (strength)
	{
		case randomizeSlight:
		{
			numberOfParametersToChange = 1;
			randomScale = 0.05;
			break;
		}
		case randomizeMedium:
		{
			numberOfParametersToChange = numberOfParameters / 100 + 2;
			randomScale = 0.3;
			break;
		}
		case randomizeHeavy:
		{
			numberOfParametersToChange = numberOfParameters / 10 + 3;
			randomScale = 1.0;
			break;
		}
	}
	numberOfParametersToChange = qMin(numberOfParametersToChange, numberOfParameters);

	QVector<int> listOfIndexes;

	for (int i = 0; i < numberOfParametersToChange; i++)
	{
		int randomIndex = 0;
		do
		{
			randomIndex =
				randomizer.Random(numberOfParameters - 2) + 1; // first string in the tree is root
		} while (listOfIndexes.contains(randomIndex));
		listOfIndexes.append(randomIndex);

		QString fullParameterName = parametersTree.GetString(randomIndex);
		qDebug() << fullParameterName;

		cParameterContainer *container = ContainerSelector(fullParameterName, params, fractal);
		int firstDashIndex = fullParameterName.indexOf("_");
		QString parametername = fullParameterName.mid(firstDashIndex + 1);
		qDebug() << container->GetContainerName() << parametername;

		cOneParameter parameter = container->GetAsOneParameter(parametername);
		enumVarType varType = parameter.GetValueType();
		qDebug() << varType;

		switch (varType)
		{
			case typeInt:
			{
				int min = parameter.Get<int>(valueMin);
				int max = parameter.Get<int>(valueMax);
				int value = parameter.Get<int>(valueActual);
				int sign = randomizer.Random(1);
				if (sign == 0 || value == min)
				{
					int range = max - value - 1;
					if (range > 0)
					{
						int r = randomizer.Random(range) * randomScale + 1;
						value += r;
					}
					else
					{
						value++;
					}
				}
				else
				{
					int range = value - min - 1;
					if (range > 0)
					{
						int r = randomizer.Random(range) * randomScale + 1;
						value -= r;
					}
					else
					{
						value--;
					}
				}

				if (value > max) value = max;
				if (value < min) value = min;

				parameter.Set(value, valueActual);
				break;
			}
			case typeDouble:
			{
				double min = parameter.Get<double>(valueMin);
				double max = parameter.Get<double>(valueMax);
				double value = parameter.Get<double>(valueActual);
				if (value == 0.0)
				{
					if (min < 0.0)
					{
						value = randomizer.Random(-randomScale, randomScale, randomScale / 1000.0);
					}
					else
					{
						value = randomizer.Random(0.0, randomScale, randomScale / 1000.0);
					}
				}
				else
				{
					if (min < 0.0)
					{
						double r = randomizer.Random(-randomScale, randomScale, randomScale / 1000.0);
						value += r;
					}
					else
					{
						double r = randomizer.Random(-randomScale, randomScale, randomScale / 1000.0);
						value = value * pow(10, r);
					}
					if (value > max) value = max;
					if (value < min) value = min;
				}
				parameter.Set(value, valueActual);
				break;
			}
			case typeString:
			{
				break;
			}
			case typeVector3:
			{
				break;
			}
			case typeVector4:
			{
				break;
			}
			case typeRgb:
			{
				break;
			}
			case typeBool:
			{
				parameter.Set(!parameter.Get<bool>(valueActual), valueActual);
				break;
			}
			default: break;
		}

		container->SetFromOneParameter(parametername, parameter);
	}
}

void cRandomizerDialog::slotClieckedHeavyRandomize()
{
	Randomize(randomizeHeavy);
}

void cRandomizerDialog::CreateParametersTreeInWidget(
	cTreeStringList *tree, const QWidget *widget, int &level, int parentId)
{
	QList<QWidget *> listChildrenWidgets =
		widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);

	if (listChildrenWidgets.size() > 0)
	{
		QSet<QString> listOfParameters;

		foreach (QWidget *w, listChildrenWidgets)
		{
			// qDebug() << QString(level, ' ') << "Widget: " << w->objectName();

			int newParentId = -1;

			CommonMyWidgetWrapper *myWidget = dynamic_cast<CommonMyWidgetWrapper *>(w);
			if (myWidget)
			{
				QString parameterName = myWidget->getFullParameterName();
				QString containerName = myWidget->getParameterContainerName();
				QString fullParameterName = containerName + "_" + parameterName;
				listOfParameters.insert(fullParameterName);

				qDebug() << QString(level, ' ') << "ParameterName: " << fullParameterName;

				int newId = tree->AddChildItem(fullParameterName, parentId);

				if (dynamic_cast<MyGroupBox *>(w))
				{
					qDebug() << QString(level, ' ') << "GroupBox:" << fullParameterName;
					newParentId = newId;
				}
			}

			// recursion
			level++;
			CreateParametersTreeInWidget(tree, w, level, (newParentId >= 0) ? newParentId : parentId);
		}
	}
	level--;
}

cParameterContainer *cRandomizerDialog::ContainerSelector(
	QString fullParameterName, cParameterContainer *params, cFractalContainer *fractal)
{
	cParameterContainer *container = nullptr;

	int firstDashIndex = fullParameterName.indexOf("_");
	QString containerName = fullParameterName.left(firstDashIndex);

	if (containerName == "main")
	{
		container = params;
	}
	else if (containerName.indexOf("fractal") >= 0)
	{
		const int index = containerName.rightRef(1).toInt();
		if (index < 4)
		{
			container = &fractal->at(index);
		}
		else
		{
			qWarning() << "cRandomizerDialog::ContainerSelector(): wrong fractal container index"
								 << containerName << index;
		}
	}
	else
	{
		qWarning() << "cRandomizerDialog::ContainerSelector(): wrong container name" << containerName;
	}

	return container;
}
