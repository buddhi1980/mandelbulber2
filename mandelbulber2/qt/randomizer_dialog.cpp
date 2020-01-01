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
#include "src/interface.hpp"

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
	connect(
		ui->pushButton_use, &QPushButton::clicked, this, &cRandomizerDialog::slotCkickedUseButton);

	for (int i = 1; i <= numberOfVersions; i++)
	{
		QString widgetName = QString("pushButton_select_%1").arg(i + 1, 2, 10, QChar('0'));
		QPushButton *button = this->findChild<QPushButton *>(widgetName);
		connect(button, &QPushButton::clicked, this, &cRandomizerDialog::slotCkickedSelectButton);
	}

	ui->previewwidget_actual->SetSize(240, 160, 2);
	for (int i = 0; i < numberOfVersions; i++)
	{
		QString widgetName = QString("previewwidget_%1").arg(i + 1, 2, 10, QChar('0'));
		cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
		previewWidget->SetSize(240, 160, 2);
	}

	// local copy of parameters
	actualParams = *gPar;
	actualFractParams = *gParFractal;
}

cRandomizerDialog::~cRandomizerDialog()
{
	delete ui;
}

void cRandomizerDialog::Randomize(enimRandomizeStrength strength)
{
	// initialize parameter containers
	listOfVersions.clear();
	for (int i = 0; i < numberOfVersions; i++)
	{
		sParameterVersion version;
		version.params = actualParams;
		version.fractParams = actualFractParams;
		listOfVersions.append(version);
	}

	ui->previewwidget_actual->AssignParameters(actualParams, actualFractParams);
	ui->previewwidget_actual->update();

	for (int i = 0; i < numberOfVersions; i++)
	{
		qDebug() << "Version " << i;

		RandomizeParameters(strength, &listOfVersions[i].params, &listOfVersions[i].fractParams);

		QString widgetName = QString("previewwidget_%1").arg(i + 1, 2, 10, QChar('0'));
		qDebug() << widgetName;
		cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
		qDebug() << previewWidget;
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

void cRandomizerDialog::RandomizeIntegerParameter(double randomScale, cOneParameter &parameter)
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
	if (min != max)
	{
		if (value > max) value = max;
		if (value < min) value = min;
	}

	parameter.Set(value, valueActual);
}

void cRandomizerDialog::RandomizeDoubleParameter(
	double randomScale, bool isAngle, cOneParameter &parameter)
{
	double min = parameter.Get<double>(valueMin);
	double max = parameter.Get<double>(valueMax);
	double value = parameter.Get<double>(valueActual);
	if (value == 0.0)
	{
		if (min < 0.0)
		{
			if (isAngle)
				value = randomizer.Random(-randomScale * 180.0, randomScale * 180.0, randomScale / 10.0);
			else
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
			double r = randomizer.Random(-randomScale * 5.0, randomScale * 5.0, randomScale / 1000.0);
			value += r;
		}
		else
		{
			double r = randomizer.Random(-randomScale, randomScale, randomScale / 1000.0);
			value = value * pow(10.0, r);
		}

		if (min != max)
		{
			if (value > max) value = max;
			if (value < min) value = min;
		}
	}
	parameter.Set(value, valueActual);
}

double cRandomizerDialog::RandomizeDoubleValue(double value, double randomScale, bool isAngle)
{
	if (value == 0)
	{
		if (isAngle)
			value = randomizer.Random(-randomScale * 180.0, randomScale * 180.0, randomScale / 10.0);
		else
			value = randomizer.Random(-randomScale, randomScale, randomScale / 1000.0);
	}
	else
	{
		double r = randomizer.Random(-randomScale, randomScale, randomScale / 1000.0);
		value = value * pow(10.0, r);
	}
	return value;
}

void cRandomizerDialog::RandomizeVector3Parameter(
	double randomScale, bool isAngle, cOneParameter &parameter)
{
	CVector3 value = parameter.Get<CVector3>(valueActual);
	value.x = RandomizeDoubleValue(value.x, randomScale, isAngle);
	value.y = RandomizeDoubleValue(value.y, randomScale, isAngle);
	value.z = RandomizeDoubleValue(value.z, randomScale, isAngle);
	parameter.Set(value, valueActual);
}

void cRandomizerDialog::RandomizeVector4Parameter(
	double randomScale, bool isAngle, cOneParameter &parameter)
{
	CVector4 value = parameter.Get<CVector4>(valueActual);
	value.x = RandomizeDoubleValue(value.x, randomScale, isAngle);
	value.y = RandomizeDoubleValue(value.y, randomScale, isAngle);
	value.z = RandomizeDoubleValue(value.z, randomScale, isAngle);
	value.w = RandomizeDoubleValue(value.w, randomScale, isAngle);
	parameter.Set(value, valueActual);
}

int cRandomizerDialog::RandomizeColorComponent(double randomScale, int value)
{
	int min = value - randomScale * 65535;
	if (min < 0) min = 0;
	int max = value + randomScale * 65535;
	if (max > 65535) max = 65535;
	int range = max - min;
	int r = randomizer.Random(range);
	value = min + r;
	if (value < 0) value = 0;
	if (value > 65536) value = 65535;
	return value;
}

void cRandomizerDialog::RandomizeRGBParameter(double randomScale, cOneParameter &parameter)
{
	sRGB value = parameter.Get<sRGB>(valueActual);
	value.R = RandomizeColorComponent(randomScale, value.R);
	value.G = RandomizeColorComponent(randomScale, value.G);
	value.B = RandomizeColorComponent(randomScale, value.B);
	parameter.Set(value, valueActual);
}

void cRandomizerDialog::RandomizeBooleanParameter(cOneParameter &parameter)
{
	parameter.Set(!parameter.Get<bool>(valueActual), valueActual);
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
			randomScale = 0.01;
			break;
		}
		case randomizeMedium:
		{
			numberOfParametersToChange = numberOfParameters / 100 + 2;
			randomScale = 0.1;
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
		int trialCounter = numberOfParameters * 10;
		do
		{
			randomIndex =
				randomizer.Random(numberOfParameters - 2) + 1; // first string in the tree is root
			trialCounter--;
		} while (listOfIndexes.contains(randomIndex) && trialCounter > 0);
		listOfIndexes.append(randomIndex);

		QString fullParameterName = parametersTree.GetString(randomIndex);
		qDebug() << fullParameterName;

		// check is parameter is enabled
		bool skip = false;

		if (!parametersTree.IsEnabled(randomIndex))
		{
			i--;
			continue;
		}

		int actualParameterIndex = randomIndex;
		int parentIndex = 0;
		do
		{
			parentIndex = parametersTree.GetParentIndex(actualParameterIndex);
			if (parentIndex >= 0)
			{
				qDebug() << "--Checking parent:" << parametersTree.GetString(parentIndex);

				if (parametersTree.GetString(parentIndex) == "root") break;

				bool hidden = !parametersTree.IsEnabled(parentIndex);
				if (hidden)
				{
					qDebug() << "is hidden";
					skip = true;
					break;
				}

				QString fullParameterNameToCheck = parametersTree.GetString(parentIndex);
				cParameterContainer *container =
					ContainerSelector(fullParameterNameToCheck, params, fractal);
				int firstDashIndex = fullParameterNameToCheck.indexOf("_");
				QString parameterName = fullParameterNameToCheck.mid(firstDashIndex + 1);
				if (!container->Get<bool>(parameterName))
				{
					skip = true;
					break;
				}
				actualParameterIndex = parentIndex;
			}
		} while (parentIndex >= 0);

		if (skip)
		{
			i--;
			continue;
		}

		// if parameter is a group then enable this group and randomize deeper parameter
		if (parametersTree.IsGroup(randomIndex))
		{
			QList<int> childrens = parametersTree.GetChildrens(randomIndex);

			int randomChildIndex;
			if (childrens.size() > 1)
			{
				randomChildIndex = randomizer.Random(childrens.size() - 1);
			}
			else
			{
				randomChildIndex = 0;
			}
			int deeperRandomIndex = childrens.at(randomChildIndex);

			// enable group
			cParameterContainer *container = ContainerSelector(fullParameterName, params, fractal);
			int firstDashIndex = fullParameterName.indexOf("_");
			QString parameterName = fullParameterName.mid(firstDashIndex + 1);
			container->Set(parameterName, true);

			// and further randomize one of deeper parameters
			fullParameterName = parametersTree.GetString(deeperRandomIndex);
			qDebug() << "----Deeper: " << fullParameterName;
		}

		RandomizeOneParameter(fullParameterName, randomScale, params, fractal);
	}
}

void cRandomizerDialog::RandomizeOneParameter(QString fullParameterName, double randomScale,
	cParameterContainer *params, cFractalContainer *fractal)
{
	cParameterContainer *container = ContainerSelector(fullParameterName, params, fractal);
	int firstDashIndex = fullParameterName.indexOf("_");
	QString parameterName = fullParameterName.mid(firstDashIndex + 1);
	qDebug() << "---Randomizing: " << container->GetContainerName() << parameterName;

	cOneParameter parameter = container->GetAsOneParameter(parameterName);
	enumVarType varType = parameter.GetValueType();
	enumMorphType morphType = parameter.GetMorphType();
	bool isAngle = false;
	if (morphType == morphLinearAngle || morphType == morphCatMullRomAngle
			|| morphType == morphAkimaAngle)
		isAngle = true;

	switch (varType)
	{
		case typeInt:
		{
			RandomizeIntegerParameter(randomScale, parameter);
			break;
		}
		case typeDouble:
		{
			RandomizeDoubleParameter(randomScale, isAngle, parameter);
			break;
		}
		case typeString:
		{
			break;
		}
		case typeVector3:
		{
			RandomizeVector3Parameter(randomScale, isAngle, parameter);
			break;
		}
		case typeVector4:
		{
			RandomizeVector4Parameter(randomScale, isAngle, parameter);
			break;
		}
		case typeRgb:
		{
			RandomizeRGBParameter(randomScale, parameter);
			break;
		}
		case typeBool:
		{
			RandomizeBooleanParameter(parameter);
			break;
		}
		default: break;
	}

	container->SetFromOneParameter(parameterName, parameter);
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

				bool hidden = w->isHidden();
				if (hidden) qDebug() << "hidden";

				int newId = tree->AddChildItem(fullParameterName, !hidden, parentId);

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

void cRandomizerDialog::slotCkickedSelectButton()
{
	QString buttonName = this->sender()->objectName();
	QString numberString = buttonName.right(2);
	int buttonNumber = numberString.toInt();

	actualParams = listOfVersions.at(buttonNumber - 1).params;
	actualFractParams = listOfVersions.at(buttonNumber - 1).fractParams;

	ui->previewwidget_actual->AssignParameters(actualParams, actualFractParams);
	ui->previewwidget_actual->update();
}

void cRandomizerDialog::slotCkickedUseButton()
{
	*gPar = actualParams;
	*gParFractal = actualFractParams;
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
	close();
}
