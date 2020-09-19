/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * Settings Randomizer
 */

#include "randomizer_dialog.h"

#include <memory>

#include "ui_randomizer_dialog.h"

#include "common_my_widget_wrapper.h"
#include "my_group_box.h"
#include "thumbnail_widget.h"

#include "src/animation_keyframes.hpp"
#include "src/cimage.hpp"
#include "src/color_gradient.h"
#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/keyframes.hpp"
#include "src/settings.hpp"
#include "src/system_data.hpp"
#include "src/wait.hpp"

#include "formula/definition/all_fractal_list.hpp"

cRandomizerDialog::cRandomizerDialog(QWidget *parent)
		: QDialog(parent), ui(new Ui::cRandomizerDialog)
{
	ui->setupUi(this);

	setModal(true);

	randomizer.Initialize(QTime::currentTime().msec());

	actualStrength = randomizeMedium;
	referenceNoise = 0.0;
	pressedUse = false;
	blockClose = false;
	keyframeMode = false;
	stopRequest = false;
	numberOfRunningJobs = 0;
	keepCount = 0;

	actualParams.reset(new cParameterContainer());
	actualFractParams.reset(new cFractalContainer());

	int baseSize = int(systemData.GetPreferredThumbnailSize());
	int sizeSetiing = enumRandomizerPreviewSize(gPar->Get<int>("randomizer_preview_size"));
	int qualitySetiing = enumRandomizerPreviewQuality(gPar->Get<int>("randomizer_preview_quality"));
	float sizeMultiply = 1.0f;
	switch (sizeSetiing)
	{
		case previewSmall: sizeMultiply = 1.0; break;
		case previewMedium: sizeMultiply = 1.5; break;
		case previewBig: sizeMultiply = 2.0; break;
		default: sizeMultiply = 1.0; break;
	}
	previewWidth = int(sizeMultiply * baseSize * 4.0 / 3.0);
	previewHeight = int(sizeMultiply * baseSize);

	qualityMultiplier = qualitySetiing + 1;

	connect(ui->pushButton_heavy, &QPushButton::clicked, this,
		&cRandomizerDialog::slotClickedHeavyRandomize);
	connect(ui->pushButton_medium, &QPushButton::clicked, this,
		&cRandomizerDialog::slotClickedMediumRandomize);
	connect(ui->pushButton_slight, &QPushButton::clicked, this,
		&cRandomizerDialog::slotClickedSlightRandomize);
	connect(
		ui->pushButton_use, &QPushButton::clicked, this, &cRandomizerDialog::slotClickedUseButton);
	connect(
		ui->pushButton_reset, &QPushButton::clicked, this, &cRandomizerDialog::slotClickedResetButton);
	connect(ui->pushButton_clean_up, &QPushButton::clicked, this, &cRandomizerDialog::slotCleanUp);
	connect(ui->pushButton_add_to_keyframes, &QPushButton::clicked, this,
		&cRandomizerDialog::slotAddToKeyframes);
	connect(
		ui->pushButton_stop, &QPushButton::clicked, this, &cRandomizerDialog::slotClickedStopButton);

	for (int i = 1; i <= numberOfVersions; i++)
	{
		QString widgetName = QString("pushButton_select_%1").arg(i, 2, 10, QChar('0'));
		QPushButton *button = this->findChild<QPushButton *>(widgetName);
		connect(button, &QPushButton::clicked, this, &cRandomizerDialog::slotClickedSelectButton);
	}

	for (int i = 1; i <= numberOfVersions; i++)
	{
		QString widgetName = QString("toolButton_save_%1").arg(i, 2, 10, QChar('0'));
		QToolButton *button = this->findChild<QToolButton *>(widgetName);
		connect(button, &QToolButton::clicked, this, &cRandomizerDialog::slotClickedSaveButton);
	}

	// local copy of parameters
	*actualParams = *gPar;
	*actualFractParams = *gParFractal;

	ui->previewwidget_actual->SetSize(previewWidth, previewHeight, qualityMultiplier);
	ui->previewwidget_actual->DisableThumbnailCache();
	ui->previewwidget_actual->AssignParameters(actualParams, actualFractParams);
	ui->previewwidget_actual->update();

	referenceSkyPreview.reset(new cThumbnailWidget());
	referenceSkyPreview->SetSize(previewWidth, previewHeight, qualityMultiplier);
	referenceSkyPreview->DisableThumbnailCache();
	referenceSkyPreview->DisableTimer();

	referenceNoisePreview.reset(new cThumbnailWidget());
	referenceNoisePreview->SetSize(previewWidth, previewHeight, qualityMultiplier);
	referenceNoisePreview->DisableThumbnailCache();
	referenceNoisePreview->DisableTimer();

	for (int i = 0; i < numberOfVersions; i++)
	{
		QString widgetName = QString("previewwidget_%1").arg(i + 1, 2, 10, QChar('0'));
		cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
		previewWidget->SetSize(previewWidth, previewHeight, qualityMultiplier);
		previewWidget->DisableThumbnailCache();
		previewWidget->UseOneCPUCore(true);

		connect(previewWidget, &cThumbnailWidget::thumbnailRendered, this,
			&cRandomizerDialog::slotPreviewRendered);
		connect(previewWidget, &cThumbnailWidget::signalFinished, this,
			&cRandomizerDialog::slotPreviewFinished);
		connect(previewWidget, &cThumbnailWidget::signalZeroDistance, this,
			&cRandomizerDialog::slotDetectedZeroDistance);
		connect(previewWidget, &cThumbnailWidget::signalTotalRenderTime, this,
			&cRandomizerDialog::slotRenderTime);

		numbersOfRepeats.append(0);
		versionsDone.append(false);
		keep.append(false);
		listsOfChangedParameters.append(QMap<QString, QString>());

		sParameterVersion version;
		*version.params = *actualParams;
		*version.fractParams = *actualFractParams;
		listOfVersions.append(version);
	}

	ui->checkBox_dont_randomize_booleans->setChecked(gPar->Get<bool>("randomizer_only_floats"));
	ui->checkBox_dont_randomize_camera->setChecked(
		gPar->Get<bool>("randomizer_dont_randomize_camera"));
	ui->checkBox_dont_randomize_camera->hide();
}

cRandomizerDialog::~cRandomizerDialog()
{
	delete ui;
}

void cRandomizerDialog::InitParameterContainers()
{
	// initialize parameter containers
	for (int i = 0; i < numberOfVersions; i++)
	{
		if (!keep[i])
		{
			sParameterVersion version;
			*listOfVersions[i].params = *actualParams;
			*listOfVersions[i].fractParams = *actualFractParams;
		}
	}
}

void cRandomizerDialog::RefreshReferenceImage()
{
	// refresh actual reference image
	blockClose = true;
	ui->previewwidget_actual->AssignParameters(actualParams, actualFractParams);
	if (!ui->previewwidget_actual->IsRendered())
	{
		ui->previewwidget_actual->slotRender();
		while (!ui->previewwidget_actual->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}
	ui->previewwidget_actual->update();
	blockClose = false;
}

void cRandomizerDialog::RefreshReferenceSkyImage()
{
	// render reference sky
	std::shared_ptr<cParameterContainer> parSky(new cParameterContainer);
	*parSky = *actualParams;
	std::shared_ptr<cFractalContainer> parFractSky(new cFractalContainer);
	*parFractSky = *actualFractParams;

	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		parSky->Set("fractal_enable", i, false);
	}
	parSky->Set("iteration_threshold_mode", false);
	// iterThresh mode needs to be disabled to render sky, otherwise DE will be zero.
	blockClose = true;
	referenceSkyPreview->AssignParameters(parSky, parFractSky);
	if (!referenceSkyPreview->IsRendered())
	{
		referenceSkyPreview->slotRender();
		while (!referenceSkyPreview->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}
	blockClose = false;
}

void cRandomizerDialog::CalcReferenceNoise()
{
	// render the same image as actual to calculate reference noise
	blockClose = true;
	referenceNoisePreview->AssignParameters(actualParams, actualFractParams);
	if (!referenceNoisePreview->IsRendered())
	{
		referenceNoisePreview->slotRender();
		while (!referenceNoisePreview->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}
	blockClose = false;
	std::shared_ptr<cImage> actualImage = ui->previewwidget_actual->GetImage();
	std::shared_ptr<cImage> noiseRefImage = referenceNoisePreview->GetImage();
	referenceNoise = noiseRefImage->VisualCompare(actualImage, true);
	// qDebug() << "Reference noise: " << referenceNoise;
}

void cRandomizerDialog::Randomize(enimRandomizeStrength strength)
{
	if (stopRequest) return;
	if (numberOfRunningJobs > 0) return;

	actualStrength = strength;
	previousListOfChangedParameters = actualListOfChangedParameters;

	gPar->Set("randomizer_only_floats", ui->checkBox_dont_randomize_booleans->isChecked());
	gPar->Set("randomizer_dont_randomize_camera", ui->checkBox_dont_randomize_camera->isChecked());

	QString progressBarText("Initializing Randomizer");
	ui->progressBar->setFormat(progressBarText);
	ui->progressBar->setValue(0.0);

	stopRequest = false;

	// update list of version which will be excluded from randomizing
	UpdateKeepList();

	// initialize parameter containers
	InitParameterContainers();

	progressText.ResetTimer();

	// refresh actual reference image
	RefreshReferenceImage();

	// render reference sky
	RefreshReferenceSkyImage();

	// render the same image as actual to calculate reference noise
	CalcReferenceNoise();

	// randomizing
	for (int i = 0; i < numberOfVersions; i++)
	{
		// qDebug() << "Version " << i;

		numbersOfRepeats[i] = 0;
		versionsDone[i] = false;

		if (!keep[i])
		{
			RandomizeParameters(strength, listOfVersions[i].params, listOfVersions[i].fractParams, i);

			QString widgetName = QString("previewwidget_%1").arg(i + 1, 2, 10, QChar('0'));
			// qDebug() << widgetName;
			cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
			// qDebug() << previewWidget;

			numberOfRunningJobs++;
			// qDebug() << numberOfRunningJobs;
			previewWidget->AssignParameters(
				listOfVersions.at(i).params, listOfVersions.at(i).fractParams);
			previewWidget->setToolTip(CreateTooltipText(listsOfChangedParameters[i]));
			previewWidget->update();
		}
	}
}

void cRandomizerDialog::UpdateKeepList()
{
	keepCount = 0;

	for (int i = 0; i < numberOfVersions; i++)
	{
		QString checkBoxKeepWidgetName = QString("checkBox_keep_%1").arg(i + 1, 2, 10, QChar('0'));
		QCheckBox *checkBoxKeep = this->findChild<QCheckBox *>(checkBoxKeepWidgetName);
		keep[i] = checkBoxKeep->isChecked();
		if (keep[i]) keepCount++;
	}
}

void cRandomizerDialog::slotClickedSlightRandomize()
{
	Randomize(randomizeSlight);
}

void cRandomizerDialog::slotClickedMediumRandomize()
{
	Randomize(randomizeMedium);
}

void cRandomizerDialog::slotClickedHeavyRandomize()
{
	Randomize(randomizeHeavy);
}

void cRandomizerDialog::AssignSourceWidget(const QWidget *sourceWidget)
{
	int level = 0;
	setWindowTitle(tr("Randomizer (for parameters from %1)").arg(sourceWidget->objectName()));
	CreateParametersTreeInWidget(&parametersTree, sourceWidget, level, 0);
}

void cRandomizerDialog::AssignParameters(const QStringList &list)
{
	int level = 0;
	setWindowTitle(tr("Randomizer (for parameters from keyframe animation)"));

	for (const QString &parameter : list)
	{
		std::shared_ptr<cParameterContainer> container =
			ContainerSelector(parameter, gPar, gParFractal);
		int firstDashIndex = parameter.indexOf("_");
		QString parameterName = parameter.mid(firstDashIndex + 1);
		enumVarType varType = container->GetVarType(parameterName);
		bool isFloat = (varType == typeDouble || varType == typeVector3 || varType == typeVector4
										|| varType == typeRgb);
		parametersTree.AddChildItem(parameter, true, isFloat, 0);
	}
	keyframeMode = true;
	ui->checkBox_dont_randomize_camera->show();
}

void cRandomizerDialog::RandomizeIntegerParameter(
	double randomScale, cOneParameter &parameter, const QString &parameterName)
{
	int min = parameter.Get<int>(valueMin);
	int max = parameter.Get<int>(valueMax);
	int value = parameter.Get<int>(valueActual);
	if (parameterName.contains(QRegularExpression("formula_\\d")))
	{
		// qDebug() << "*********** randomizing fractal *******************";
		// find fractal on the list
		int indexOnFractalList = -1;
		for (int i = 0; i < newFractalList.size(); i++)
		{
			if (newFractalList.at(i)->getInternalId() == value)
			{
				indexOnFractalList = i;
				break;
			}
		}
		int minIndex = indexOnFractalList - newFractalList.size() * randomScale;
		minIndex = qMax(minIndex, 0);
		int maxIndex = indexOnFractalList + newFractalList.size() * randomScale;
		maxIndex = qMin(maxIndex, newFractalList.size() - 1);

		int r = randomizer.Random(maxIndex - minIndex) + minIndex;
		value = newFractalList.at(r)->getInternalId();
	}
	else
	{
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
		if (isAngle)
		{
			value = fmod(value + 3 * 180.0, 2 * 180.0) - 180.0;
		}
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

int cRandomizerDialog::RandomizeColor16Component(double randomScale, int value)
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

int cRandomizerDialog::RandomizeColor8Component(double randomScale, int value)
{
	int min = value - randomScale * 255;
	if (min < 0) min = 0;
	int max = value + randomScale * 255;
	if (max > 255) max = 255;
	int range = max - min;
	int r = randomizer.Random(range);
	value = min + r;
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	return value;
}

void cRandomizerDialog::RandomizeRGBParameter(double randomScale, cOneParameter &parameter)
{
	sRGB value = parameter.Get<sRGB>(valueActual);
	value.R = RandomizeColor16Component(randomScale, value.R);
	value.G = RandomizeColor16Component(randomScale, value.G);
	value.B = RandomizeColor16Component(randomScale, value.B);
	parameter.Set(value, valueActual);
}

void cRandomizerDialog::RandomizeBooleanParameter(cOneParameter &parameter)
{
	parameter.Set(!parameter.Get<bool>(valueActual), valueActual);
}

void cRandomizerDialog::RandomizeStringParameter(double randomScale, cOneParameter &parameter)
{
	if (parameter.IsGradient())
	{
		cColorGradient gradient;
		gradient.SetColorsFromString(parameter.Get<QString>(valueActual));
		int numberOfColors = gradient.GetNumberOfColors();

		if (numberOfColors == 2)
		{
			int numberOfColors = randomizer.Random(18) + 2;
			gradient.DeleteAll();
			for (int i = 0; i < numberOfColors; i++)
			{
				sRGB color(randomizer.Random(255), randomizer.Random(255), randomizer.Random(255));
				float position = randomizer.Random(10000) / 10000.0f;
				if (i == 0)
				{
					gradient.AddColor(color, 0.0f);
					gradient.AddColor(color, 1.0f);
				}
				else
				{
					gradient.AddColor(color, position);
				}
			}
		}
		else
		{
			for (int i = 0; i < numberOfColors; i++)
			{
				sRGB color = gradient.GetColorByIndex(i);
				color.R = RandomizeColor8Component(randomScale, color.R);
				color.G = RandomizeColor8Component(randomScale, color.G);
				color.B = RandomizeColor8Component(randomScale, color.B);
				gradient.ModifyColor(i, color);

				if (i == 0)
				{
					gradient.ModifyColor(1, color);
					i++;
				}

				if (i >= 2)
				{
					float position = gradient.GetPositionByIndex(i);
					float delta = 1.0 / numberOfColors * randomScale;
					float r = randomizer.Random(-delta, delta, delta / 100.0);
					position += r;
					if (position < 0.0) position = 0.0;
					if (position > 1.0) position = 1.0;
					gradient.ModifyPosition(i, position);
				}
			}
		}
		QString gradientString = gradient.GetColorsAsString();
		parameter.Set(gradientString, valueActual);
	}
}

void cRandomizerDialog::RandomizeParameters(enimRandomizeStrength strength,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal,
	int widgetIndex)
{
	listsOfChangedParameters[widgetIndex].clear();

	int numberOfParameters = parametersTree.GetSize();
	bool onlyFloatsOption = ui->checkBox_dont_randomize_booleans->isChecked();
	bool skipCameraOption = ui->checkBox_dont_randomize_camera->isChecked();

	if (numberOfParameters <= 1) return; // no parameters to random (only "root")

	int numberOfParametersToChange = 0;
	double randomScale = 1.0;

	switch (strength)
	{
		case randomizeSlight:
		{
			numberOfParametersToChange = 1;
			randomScale = 0.1;
			break;
		}
		case randomizeMedium:
		{
			numberOfParametersToChange = numberOfParameters / 100 + 2;
			randomScale = 0.5;
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
	int globalTrialCounter = numberOfParametersToChange * 100;

	for (int i = 0; i < numberOfParametersToChange; i++)
	{
		globalTrialCounter--;
		if (globalTrialCounter <= 0) break;

		int randomIndex = 0;
		int trialCounter = numberOfParameters * 10;
		do
		{
			if (numberOfParameters > 2)
			{
				randomIndex =
					randomizer.Random(numberOfParameters - 2) + 1; // first string in the tree is root
			}
			else
			{
				randomIndex = 1; // there is only parameter #1
			}
			trialCounter--;
		} while (listOfIndexes.contains(randomIndex) && trialCounter > 0);
		listOfIndexes.append(randomIndex);

		QString fullParameterName = parametersTree.GetString(randomIndex);
		// qDebug() << fullParameterName;

		// check if camera parameter
		bool skipCamera =
			(skipCameraOption
				&& (fullParameterName.contains("camera") || fullParameterName.contains("target")));

		// check is parameter is enabled
		bool skip = false;

		if (!parametersTree.IsEnabled(randomIndex)
				|| (onlyFloatsOption && !parametersTree.IsFloat(randomIndex)) || skipCamera)
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
				// qDebug() << "--Checking parent:" << parametersTree.GetString(parentIndex);

				if (parametersTree.GetString(parentIndex) == "root") break;

				bool hidden = !parametersTree.IsEnabled(parentIndex);
				if (hidden)
				{
					// qDebug() << "is hidden";
					skip = true;
					break;
				}

				QString fullParameterNameToCheck = parametersTree.GetString(parentIndex);
				std::shared_ptr<cParameterContainer> container =
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
			std::shared_ptr<cParameterContainer> container =
				ContainerSelector(fullParameterName, params, fractal);
			int firstDashIndex = fullParameterName.indexOf("_");
			QString parameterName = fullParameterName.mid(firstDashIndex + 1);
			container->Set(parameterName, true);

			// and further randomize one of deeper parameters
			fullParameterName = parametersTree.GetString(deeperRandomIndex);
			// qDebug() << "----Deeper: " << fullParameterName;
		}

		RandomizeOneParameter(fullParameterName, randomScale, params, fractal, widgetIndex);
	}
}

void cRandomizerDialog::RandomizeOneParameter(QString fullParameterName, double randomScale,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal,
	int widgetIndex)
{
	std::shared_ptr<cParameterContainer> container =
		ContainerSelector(fullParameterName, params, fractal);
	int firstDashIndex = fullParameterName.indexOf("_");
	QString parameterName = fullParameterName.mid(firstDashIndex + 1);
	// qDebug() << "---Randomizing: " << container->GetContainerName() << parameterName;

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
			RandomizeIntegerParameter(randomScale, parameter, parameterName);
			break;
		}
		case typeDouble:
		{
			RandomizeDoubleParameter(randomScale, isAngle, parameter);
			break;
		}
		case typeString:
		{
			RandomizeStringParameter(randomScale, parameter);
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

	if (!listsOfChangedParameters[widgetIndex].contains(fullParameterName))
	{
		QString parameterValueString = container->Get<QString>(parameterName);
		listsOfChangedParameters[widgetIndex].insert(fullParameterName, parameterValueString);
	}
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

				// exceptions
				if (parameterName == "") continue;
				std::shared_ptr<cParameterContainer> container =
					ContainerSelector(fullParameterName, gPar, gParFractal);
				if (container->GetParameterType(parameterName) != parameterContainer::paramStandard)
					continue;
				if (parameterName.contains("material_id")) continue;
				if (parameterName.contains("analyticDE_")) continue;
				if (parameterName.contains("flight_")) continue;
				if (parameterName.contains("keyframe_")) continue;
				if (parameterName.contains("voxel_")) continue;
				if (parameterName.contains("file")) continue;

				if (parameterName == "info") continue;
				if (parameterName == "image_width") continue;
				if (parameterName == "image_height") continue;
				if (parameterName == "random_lights_number") continue;

				listOfParameters.insert(fullParameterName);

				// qDebug() << QString(level, ' ') << "ParameterName: " << fullParameterName;

				bool hidden = w->isHidden();
				// if (hidden) qDebug() << "hidden";

				enumVarType varType = container->GetVarType(parameterName);
				bool isFloat = (varType == typeDouble || varType == typeVector3 || varType == typeVector4
												|| varType == typeRgb);

				int newId = tree->AddChildItem(fullParameterName, !hidden, isFloat, parentId);

				if (dynamic_cast<MyGroupBox *>(w))
				{
					// qDebug() << QString(level, ' ') << "GroupBox:" << fullParameterName;
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

std::shared_ptr<cParameterContainer> cRandomizerDialog::ContainerSelector(QString fullParameterName,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal)
{
	std::shared_ptr<cParameterContainer> container = nullptr;

	int firstDashIndex = fullParameterName.indexOf("_");
	QString containerName = fullParameterName.left(firstDashIndex);

	if (containerName == "main")
	{
		container = params;
	}
	else if (containerName.indexOf("fractal") >= 0)
	{
		const int index = containerName.rightRef(1).toInt();
		if (index < NUMBER_OF_FRACTALS)
		{
			container = fractal->at(index);
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

void cRandomizerDialog::slotClickedSelectButton()
{
	QString buttonName = this->sender()->objectName();
	QString numberString = buttonName.right(2);
	int buttonNumber = numberString.toInt();

	if (listOfVersions.count() > 0)
	{
		if (listOfVersions[buttonNumber - 1].params->GetCount() > 0)
		{
			*actualParams = *listOfVersions.at(buttonNumber - 1).params;
			*actualFractParams = *listOfVersions.at(buttonNumber - 1).fractParams;

			ui->previewwidget_actual->AssignParameters(actualParams, actualFractParams);
			ui->previewwidget_actual->update();

			// update main list of changed parameters
			actualListOfChangedParameters =
				previousListOfChangedParameters; // to prevent adding more setting
																				 // in case of multiple selections
			QMapIterator<QString, QString> i(listsOfChangedParameters[buttonNumber - 1]);
			while (i.hasNext())
			{
				i.next();
				if (!actualListOfChangedParameters.contains(i.key()))
				{
					actualListOfChangedParameters.insert(i.key(), i.value());
				}
			}
			ui->previewwidget_actual->setToolTip(CreateTooltipText(actualListOfChangedParameters));
		}
	}
}

void cRandomizerDialog::slotClickedSaveButton()
{
	QString buttonName = this->sender()->objectName();
	QString numberString = buttonName.right(2);
	int buttonNumber = numberString.toInt();

	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(
		listOfVersions.at(buttonNumber - 1).params, listOfVersions.at(buttonNumber - 1).fractParams);

	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save settings..."));
	dialog.setDefaultSuffix("fract");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		parSettings.SaveToFile(filename);
		systemData.lastSettingsFile = filename;
	}
}

void cRandomizerDialog::slotClickedUseButton()
{
	pressedUse = true;
	// copy parameters one by one to not loose new animBySoundParameters
	QList<QString> listOfParameters = actualParams->GetListOfParameters();
	for (QString parameterName : listOfParameters)
	{
		gPar->Copy(parameterName, actualParams);
	}
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		QList<QString> listOfFractalParameters = actualFractParams->at(i)->GetListOfParameters();
		for (QString parameterName : listOfFractalParameters)
		{
			gParFractal->at(i)->Copy(parameterName, actualFractParams->at(i));
		}
	}
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
	gPar->Set("randomizer_only_floats", ui->checkBox_dont_randomize_booleans->isChecked());
	gPar->Set("randomizer_dont_randomize_camera", ui->checkBox_dont_randomize_camera->isChecked());

	close();
}

void cRandomizerDialog::slotPreviewRendered()
{
	if (stopRequest) return;

	QString previewName = this->sender()->objectName();
	QString numberString = previewName.right(2);
	int previewNumber = numberString.toInt();

	// qDebug() << "Preview finished " << previewNumber;

	cThumbnailWidget *widget = qobject_cast<cThumbnailWidget *>(this->sender());
	widget->update();
	if (widget)
	{
		std::shared_ptr<cImage> image = widget->GetImage();
		std::shared_ptr<cImage> actualImage = ui->previewwidget_actual->GetImage();
		std::shared_ptr<cImage> skyImage = referenceSkyPreview->GetImage();
		double difference = image->VisualCompare(actualImage, true) - referenceNoise;
		double differenceSky = image->VisualCompare(skyImage, true) - referenceNoise;

		// compare with all another versions
		for (int i = 1; i <= numberOfVersions; i++)
		{
			if (i == previewNumber) continue; // do not compare with the same image
			if (versionsDone.at(i - 1))
			{
				QString widgetName = QString("previewwidget_%1").arg(i, 2, 10, QChar('0'));
				cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
				std::shared_ptr<cImage> imageFromOtherVersion = previewWidget->GetImage();
				double diffOther =
					(image->VisualCompare(imageFromOtherVersion, false) - referenceNoise) / 3.0;
				// divided by 3 to make bigger differences between versions
				difference = min(difference, diffOther);
				// qDebug() << i << diffOther << difference;
			}
		}

		int numberOfRepeats = numbersOfRepeats.at(previewNumber - 1);
		// qDebug() << "Differences: " << referenceNoise << difference << differenceSky <<
		// numberOfRepeats;

		if (difference < referenceNoise * 3 || differenceSky < referenceNoise * 3)
		{
			if (numberOfRepeats < 10)
			{
				numberOfRepeats++;
				numbersOfRepeats[previewNumber - 1] = numberOfRepeats;

				*listOfVersions[previewNumber - 1].params = *actualParams;
				*listOfVersions[previewNumber - 1].fractParams = *actualFractParams;

				RandomizeParameters(actualStrength, listOfVersions[previewNumber - 1].params,
					listOfVersions[previewNumber - 1].fractParams, previewNumber - 1);

				numberOfRunningJobs++;

				widget->AssignParameters(listOfVersions.at(previewNumber - 1).params,
					listOfVersions.at(previewNumber - 1).fractParams);
				widget->setToolTip(CreateTooltipText(listsOfChangedParameters[previewNumber - 1]));
				widget->update();
			}
			else
			{
				versionsDone[previewNumber - 1] = true;
			}
		}
		else
		{
			versionsDone[previewNumber - 1] = true;
		}
	}

	int doneCount = versionsDone.count(true);
	double progress = double(doneCount) / (numberOfVersions - keepCount);
	UpdateProgressBar(progress);
}

void cRandomizerDialog::slotDetectedZeroDistance()
{
	QString previewName = this->sender()->objectName();
	QString numberString = previewName.right(2);
	int previewNumber = numberString.toInt();
	cThumbnailWidget *widget = qobject_cast<cThumbnailWidget *>(this->sender());
	// qDebug() << "ZERO DISTANCE DETECTED " << previewNumber;

	int numberOfRepeats = numbersOfRepeats.at(previewNumber - 1);
	if (numberOfRepeats < 100 && !stopRequest)
	{
		numberOfRepeats++;
		numbersOfRepeats[previewNumber - 1] = numberOfRepeats;

		*listOfVersions[previewNumber - 1].params = *actualParams;
		*listOfVersions[previewNumber - 1].fractParams = *actualFractParams;

		RandomizeParameters(actualStrength, listOfVersions[previewNumber - 1].params,
			listOfVersions[previewNumber - 1].fractParams, previewNumber - 1);

		numberOfRunningJobs++;

		widget->AssignParameters(listOfVersions.at(previewNumber - 1).params,
			listOfVersions.at(previewNumber - 1).fractParams);
		widget->setToolTip(CreateTooltipText(listsOfChangedParameters[previewNumber - 1]));
		widget->update();
	}
}

void cRandomizerDialog::closeEvent(QCloseEvent *event)
{
	if (blockClose)
	{
		event->ignore();
		return;
	}

	if (pressedUse)
	{
		event->accept();
	}
	else
	{
		event->ignore();
		if (QMessageBox::Yes
					== QMessageBox::question(this, tr("Close Confirmation"), tr("Exit Randomizer?"),
							 QMessageBox::Yes | QMessageBox::No)
				&& !blockClose)
		{
			StopAll();
			event->accept();
		}
	}
}

void cRandomizerDialog::UpdateProgressBar(double progress)
{
	QString progressBarText = progressText.getText(progress);
	ui->progressBar->setFormat(progressBarText);
	ui->progressBar->setValue(progress * 1000);
}

void cRandomizerDialog::slotClickedResetButton()
{
	*actualParams = *gPar;
	*actualFractParams = *gParFractal;
	blockClose = true;
	ui->previewwidget_actual->AssignParameters(actualParams, actualFractParams);
	if (!ui->previewwidget_actual->IsRendered())
	{
		ui->previewwidget_actual->slotRender();
		while (!ui->previewwidget_actual->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}
	ui->previewwidget_actual->update();
	blockClose = false;
	actualListOfChangedParameters.clear();
}

QString cRandomizerDialog::CreateTooltipText(const QMap<QString, QString> &list)
{
	QString text = "List of randomized parameters:\n";
	QMapIterator<QString, QString> i(list);
	while (i.hasNext())
	{
		i.next();
		text += QString("%1 = %2\n").arg(i.key()).arg(i.value());
	}
	return text;
}

void cRandomizerDialog::slotRenderTime(double time)
{
	QString previewName = this->sender()->objectName();
	QString numberString = previewName.right(2);
	int previewNumber = numberString.toInt();

	QString widgetName = QString("groupBox_version_%1").arg(previewNumber, 2, 10, QChar('0'));
	QGroupBox *groupboxWidget = this->findChild<QGroupBox *>(widgetName);

	QString versionTimeString;
	versionTimeString = tr("Version %1, time %2s").arg(previewNumber).arg(time, 0, 'f', 1);

	groupboxWidget->setTitle(versionTimeString);
}

void cRandomizerDialog::slotCleanUp()
{
	if (numberOfRunningJobs > 0) return;
	stopRequest = false;

	QString progressBarText("Initializing cleaning up");
	ui->progressBar->setFormat(progressBarText);
	ui->progressBar->setValue(0.0);

	progressText.ResetTimer();

	// refresh actual reference image
	RefreshReferenceImage();

	// render the same image as actual to calculate reference noise
	CalcReferenceNoise();

	std::shared_ptr<cParameterContainer> actualParamsCleaned(new cParameterContainer);
	*actualParamsCleaned = *actualParams;
	std::shared_ptr<cFractalContainer> actualFractParamsCleaned(new cFractalContainer);
	*actualFractParamsCleaned = *actualFractParams;

	QList<QString> list = actualListOfChangedParameters.keys();

	std::unique_ptr<cThumbnailWidget> cleanedPreview(new cThumbnailWidget());
	cleanedPreview->SetSize(previewWidth, previewHeight, qualityMultiplier);
	cleanedPreview->DisableThumbnailCache();
	cleanedPreview->DisableTimer();

	for (int i = 0; i < list.size(); i++)
	{
		if (stopRequest) break;

		QString fullParameterName = list.at(i);
		int firstDashIndex = fullParameterName.indexOf("_");
		QString parameterName = fullParameterName.mid(firstDashIndex + 1);

		std::shared_ptr<cParameterContainer> containerOrigin =
			ContainerSelector(fullParameterName, gPar, gParFractal);

		std::shared_ptr<cParameterContainer> containerCleaned =
			ContainerSelector(fullParameterName, actualParamsCleaned, actualFractParamsCleaned);

		std::shared_ptr<cParameterContainer> containerActual =
			ContainerSelector(fullParameterName, actualParams, actualFractParams);

		cOneParameter parameterOrigin = containerOrigin->GetAsOneParameter(parameterName);
		containerCleaned->SetFromOneParameter(parameterName, parameterOrigin);

		// render cleaned settings
		blockClose = true;
		cleanedPreview->AssignParameters(actualParamsCleaned, actualFractParamsCleaned);
		if (!cleanedPreview->IsRendered())
		{
			cleanedPreview->slotRender();
			while (!cleanedPreview->IsRendered())
			{
				QApplication::processEvents();
				Wait(10);
			}
		}
		std::shared_ptr<cImage> actualImage = ui->previewwidget_actual->GetImage();
		std::shared_ptr<cImage> cleanedImage = cleanedPreview->GetImage();
		double diff = cleanedImage->VisualCompare(actualImage, false);
		blockClose = false;

		// retrieve randomized parameter because change was significant
		if (diff > referenceNoise * 3)
		{
			cOneParameter parameterActual = containerActual->GetAsOneParameter(parameterName);
			containerCleaned->SetFromOneParameter(parameterName, parameterActual);
		}
		else
		{
			actualListOfChangedParameters.remove(fullParameterName);
		}

		double progress = double(i + 1) / (list.size());
		UpdateProgressBar(progress);
		QApplication::processEvents();
	}

	*actualParams = *actualParamsCleaned;
	*actualFractParams = *actualFractParamsCleaned;

	// refresh actual reference image
	RefreshReferenceImage();
	ui->previewwidget_actual->setToolTip(CreateTooltipText(actualListOfChangedParameters));
}

void cRandomizerDialog::slotAddToKeyframes()
{
	pressedUse = true;

	QList<QString> list = actualListOfChangedParameters.keys();

	for (int i = 0; i < list.size(); i++)
	{
		QString fullParameterName = list.at(i);
		int firstDashIndex = fullParameterName.indexOf("_");
		QString parameterName = fullParameterName.mid(firstDashIndex + 1);

		std::shared_ptr<cParameterContainer> container =
			ContainerSelector(fullParameterName, gPar, gParFractal);

		if (gKeyframes->IndexOnList(parameterName, container->GetContainerName()) == -1)
		{
			gKeyframes->AddAnimatedParameter(parameterName, container->GetAsOneParameter(parameterName));
		}
	}

	// copy parameters one by one to not loose new animBySoundParameters
	QList<QString> listOfParameters = actualParams->GetListOfParameters();
	for (QString parameterName : listOfParameters)
	{
		gPar->Copy(parameterName, actualParams);
	}
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		QList<QString> listOfFractalParameters = actualFractParams->at(i)->GetListOfParameters();
		for (QString parameterName : listOfFractalParameters)
		{
			gParFractal->at(i)->Copy(parameterName, actualFractParams->at(i));
		}
	}

	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);

	gKeyframeAnimation->RefreshTable();
	gKeyframeAnimation->slotAddKeyframe();
}

void cRandomizerDialog::slotClickedStopButton()
{
	StopAll();
}

void cRandomizerDialog::slotPreviewFinished()
{
	numberOfRunningJobs--;
	if (numberOfRunningJobs == 0)
	{
		if (stopRequest)
		{
			ui->progressBar->setFormat("Stopped");
		}
		stopRequest = false;
	}
}

void cRandomizerDialog::StopAll()
{
	stopRequest = true;

	ui->progressBar->setFormat("Stopping");

	referenceSkyPreview->StopRequest();
	referenceNoisePreview->StopRequest();
	for (int i = 0; i < numberOfVersions; i++)
	{
		QString widgetName = QString("previewwidget_%1").arg(i + 1, 2, 10, QChar('0'));
		cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
		previewWidget->StopRequest();
	}
	ui->previewwidget_actual->StopRequest();
}
