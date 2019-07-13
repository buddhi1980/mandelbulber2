/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * cGradientEditWidget class - widget for editing gradients of colors
 */

#include "gradient_edit_widget.h"

#include <QColorDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QtWidgets>

#include "preview_file_dialog.h"

#include "src/common_math.h"
#include "src/error_message.hpp"
#include "src/parameters.hpp"
#include "src/random.hpp"
#include "src/system.hpp"

cGradientEditWidget::cGradientEditWidget(QWidget *parent)
		: QWidget(parent), CommonMyWidgetWrapper(this)
{
	viewMode = false;
	mouseDragStarted = false;
	pressedColorIndex = 0;
	dragStartX = 0;
	grayscale = false;

	fixHeight = int(systemData.GetPreferredThumbnailSize() / 1.4);
	setFixedHeight(fixHeight);

	buttonWidth = fixHeight / 8;
	if (buttonWidth % 2 == 0) buttonWidth += 1; // to always have odd width

	margins = buttonWidth / 2 + 2;
	toolbarHeight = int(fixHeight / 3.5);

	buttonRandomColors = new QToolButton(this);
	AddToolButton(buttonRandomColors, margins, ":gradient/icons/dice_colors.svg");
	buttonRandomColorsAndPositions = new QToolButton(this);
	AddToolButton(buttonRandomColorsAndPositions, margins + (toolbarHeight + 2) * 1,
		":gradient/icons/dice_colors_and_pos.svg");
	buttonBrightnessInc = new QToolButton(this);
	AddToolButton(buttonBrightnessInc, margins + (toolbarHeight + 2) * 2,
		":gradient/icons/gradient-brighter.svg");
	buttonBrightnessDec = new QToolButton(this);
	AddToolButton(
		buttonBrightnessDec, margins + (toolbarHeight + 2) * 3, ":gradient/icons/gradient-darker.svg");
	buttonSaturationInc = new QToolButton(this);
	AddToolButton(buttonSaturationInc, margins + (toolbarHeight + 2) * 4,
		":gradient/icons/gradient-high-saturation.svg");
	buttonSaturationDec = new QToolButton(this);
	AddToolButton(buttonSaturationDec, margins + (toolbarHeight + 2) * 5,
		":gradient/icons/gradient-low-saturation.svg");

	connect(buttonRandomColors, SIGNAL(clicked()), this, SLOT(pressedButtonRandomColors()));
	connect(buttonRandomColorsAndPositions, SIGNAL(clicked()), this,
		SLOT(pressedButtonRandomColorsAndPositions()));
	connect(buttonBrightnessInc, SIGNAL(clicked()), this, SLOT(pressedButtonBrightnessInc()));
	connect(buttonBrightnessDec, SIGNAL(clicked()), this, SLOT(pressedButtonBrightnessDec()));
	connect(buttonSaturationInc, SIGNAL(clicked()), this, SLOT(pressedButtonSaturationInc()));
	connect(buttonSaturationDec, SIGNAL(clicked()), this, SLOT(pressedButtonSaturationDec()));

	buttonRandomColors->setToolTip(
		tr("Randomize all colors in gradient without changing positions and number of colors"));
	buttonRandomColorsAndPositions->setToolTip(
		tr("Randomize all colors, positions, and number of colors in gradient"));
	buttonBrightnessInc->setToolTip(tr("Increase brightness of gradient"));
	buttonBrightnessDec->setToolTip(tr("Decrease brightness of gradient"));
	buttonSaturationInc->setToolTip(tr("Increase saturation of colors in gradient"));
	buttonSaturationDec->setToolTip(tr("Decrease saturation of colors in gradient"));
}

cGradientEditWidget::~cGradientEditWidget()
{
	// nothing to destroy
}

void cGradientEditWidget::SetGrayscale()
{
	buttonSaturationInc->disconnect();
	buttonSaturationDec->disconnect();
	delete buttonSaturationInc;
	delete buttonSaturationDec;
	grayscale = true;
	gradient.SetGrayscale();
}

void cGradientEditWidget::AddToolButton(QToolButton *toolbutton, int position, QString iconName)
{
	toolbutton->setObjectName("button");
	toolbutton->setFixedSize(toolbarHeight, toolbarHeight);
	QIcon iconRandomColors = QIcon(iconName);
	toolbutton->setIcon(iconRandomColors);
	toolbutton->setIconSize(QSize(toolbarHeight - 4, toolbarHeight - 4));
	toolbutton->move(position, 0);
	toolbutton->show();
}

void cGradientEditWidget::SetViewModeOnly()
{
	viewMode = true;
	setFixedHeight(systemData.GetPreferredThumbnailSize() / 4);
	margins = 0;
}

void cGradientEditWidget::paintEvent(QPaintEvent *event)
{
	GetDefault();

	QWidget::paintEvent(event);

	int gradientWidth = width() - 2 * margins;
	if (gradientWidth < 2) gradientWidth = 2;
	int gradientHeight = (viewMode) ? height() : (height() - toolbarHeight) / 2;

	QPainter painter(this);
	QVector<sRGB> grad = gradient.GetGradient(gradientWidth, false);

	for (int x = 0; x < grad.size(); x++)
	{
		QColor color(QColor(grad[x].R, grad[x].G, grad[x].B));
		painter.setPen(color);
		painter.drawLine(x + margins, toolbarHeight, x + margins, toolbarHeight + gradientHeight);
	}

	if (!viewMode)
	{
		QList<cColorGradient::sColor> listOfColors = gradient.GetListOfColors();
		for (cColorGradient::sColor posColor : listOfColors)
		{
			PaintButton(posColor, painter);
		}
	}
}

int cGradientEditWidget::CalcButtonPosition(float position)
{
	return int(margins + position * (width() - 2 * margins - 1));
}

void cGradientEditWidget::PaintButton(const cColorGradient::sColor &posColor, QPainter &painter)
{
	int buttonPosition = CalcButtonPosition(posColor.position);

	int buttonTop = (height() - toolbarHeight) / 2 + toolbarHeight + buttonWidth / 2;

	QRect rect(QPoint(buttonPosition - buttonWidth / 2, buttonTop),
		QPoint(buttonPosition + buttonWidth / 2, height() - 2));

	QColor color(posColor.color.R, posColor.color.G, posColor.color.B);

	QBrush brush(color, Qt::SolidPattern);
	painter.fillRect(rect, brush);

	QVector<QPoint> triangle = {QPoint(buttonPosition, buttonTop - buttonWidth / 2),
		QPoint(buttonPosition - buttonWidth / 2, buttonTop),
		QPoint(buttonPosition + buttonWidth / 2, buttonTop)};
	QPolygon pTriangle(triangle);
	QPainterPath pathTriangle;
	pathTriangle.addPolygon(pTriangle);
	painter.fillPath(pathTriangle, brush);

	int avgColor = (posColor.color.R + posColor.color.G + posColor.color.B) / 3;
	if (avgColor > 100)
		painter.setPen(Qt::black);
	else
		painter.setPen(Qt::white);

	painter.drawRect(rect);
}

int cGradientEditWidget::FindButtonAtPosition(int x)
{
	QList<cColorGradient::sColor> listOfColors = gradient.GetListOfColors();

	for (int i = listOfColors.size() - 1; i >= 0; i--)
	{
		int xButton = CalcButtonPosition(listOfColors[i].position);
		if ((x > xButton - buttonWidth / 2) && (x <= xButton + buttonWidth / 2))
		{
			return i;
		}
	}
	return -1; //-1 means nothing found
}

void cGradientEditWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (pressedColorIndex >= 2)
	{
		if (event->x() != dragStartX)
		{
			mouseDragStarted = true;
		}

		if (mouseDragStarted)
		{
			float pos = float(event->x() - margins) / (width() - 2 * margins - 1);
			gradient.ModifyPosition(pressedColorIndex, pos);
			emit update();
		}
	}
}

void cGradientEditWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (viewMode)
		{
			emit openEditor();
		}
		else
		{
			int mouseX = event->x();
			int mouseY = event->y();

			if (mouseY > height() / 2)
			{
				int index = FindButtonAtPosition(mouseX);

				dragStartX = mouseX;
				pressedColorIndex = index;
			}
		}
	}
}

void cGradientEditWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (!viewMode)
		{
			if (pressedColorIndex >= 0 && !mouseDragStarted)
			{
				QList<cColorGradient::sColor> listOfColors = gradient.GetListOfColors();

				QColorDialog colorDialog(this);
				colorDialog.setOption(QColorDialog::DontUseNativeDialog);
				sRGB colorRGB = listOfColors[pressedColorIndex].color;
				QColor color(colorRGB.R, colorRGB.G, colorRGB.B);
				colorDialog.setCurrentColor(color);
				colorDialog.setWindowTitle(
					tr("Edit color #%1").arg(QString::number(pressedColorIndex + 1)));
				if (colorDialog.exec() == QDialog::Accepted)
				{
					color = colorDialog.currentColor();
					colorRGB = sRGB(color.red(), color.green(), color.blue());
					gradient.ModifyColor(pressedColorIndex, colorRGB);

					if (pressedColorIndex == 0) gradient.ModifyColor(1, colorRGB);

					if (pressedColorIndex == 1) gradient.ModifyColor(0, colorRGB);

					emit update();
				}
			}

			pressedColorIndex = -1;
			mouseDragStarted = false;
		}
	}
}

void cGradientEditWidget::SetColors(const QString &colorsString)
{
	gradient.SetColorsFromString(colorsString);
	emit update();
}

void cGradientEditWidget::AddColor(QContextMenuEvent *event)
{
	int xClick = event->x();
	int gradientWidth = width() - 2 * margins;
	float pos = float(xClick - margins) / gradientWidth;
	gradient.SortGradient();
	sRGB color = gradient.GetColor(pos, false);
	gradient.AddColor(color, pos);
	emit update();
}

void cGradientEditWidget::RemoveColor(QContextMenuEvent *event)
{
	int xClick = event->x();
	int index = FindButtonAtPosition(xClick);
	if (index >= 2)
	{
		gradient.RemoveColor(index);
	}
	emit update();
}

void cGradientEditWidget::Clear()
{
	gradient.DeleteAndKeepTwo();
	emit update();
}

void cGradientEditWidget::ChangeNumberOfColors()
{
	bool ok;
	int newNumber = QInputDialog::getInt(this, tr("Change number of colors"),
		tr("New number of colors:"), gradient.GetNumberOfColors() - 1, 1, 100, 1, &ok);
	if (ok && newNumber != gradient.GetNumberOfColors() - 1)
	{
		gradient.SortGradient();
		cColorGradient newGradient;
		if (grayscale) newGradient.SetGrayscale();
		newGradient.DeleteAll();
		float step = 1.0f / newNumber;
		for (int i = 0; i < newNumber; i++)
		{
			if (i == 0)
			{
				sRGB color = gradient.GetColor(0.0f, false);
				newGradient.AddColor(color, 0.0f);
				newGradient.AddColor(color, 1.0f);
			}
			else
			{
				float pos = step * i;
				sRGB color = gradient.GetColor(pos, false);
				newGradient.AddColor(color, pos);
			}
		}
		gradient = newGradient;
	}
	emit update();
}

void cGradientEditWidget::GrabColors()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(QDir::toNativeSeparators(systemData.GetImagesFolder() + QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.lastImagePaletteFile));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select image to grab colors..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());

		QImage imagePalette(filename);

		if (!imagePalette.isNull())
		{
			int width = imagePalette.width();
			int height = imagePalette.height();
			int numberOfColors = gradient.GetNumberOfColors();
			gradient.DeleteAll();

			float step = 1.0f / numberOfColors;

			for (int i = 0; i < numberOfColors; i++)
			{
				double angle = double(i) / numberOfColors * M_PI * 2.0;
				double x = width / 2 + cos(angle) * width * 0.4;
				double y = height / 2 + sin(angle) * height * 0.4;
				QRgb pixel = imagePalette.pixel(int(x), int(y));
				sRGB pixelRGB(qRed(pixel), qGreen(pixel), qBlue(pixel));

				if (i == 0)
				{
					gradient.AddColor(pixelRGB, 0.0);
					gradient.AddColor(pixelRGB, 1.0);
				}
				else
				{
					float pos = step * i;
					gradient.AddColor(pixelRGB, pos);
				}
			}
		}

		systemData.lastImagePaletteFile = filename;
	}

	emit update();
}

void cGradientEditWidget::LoadColors()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Gradients (*.gradient *.txt)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastGradientFile).absolutePath()));
	dialog.selectFile(
		QDir::toNativeSeparators(QFileInfo(systemData.lastGradientFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load gradient..."));
	dialog.setDefaultSuffix("gradient");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		systemData.lastGradientFile = filename;

		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			QString stringLoaded;
			QTextStream stream(&file);
			stringLoaded = stream.readLine();

			bool result = DecodeGradientFromFile(stringLoaded);
			if (!result)
			{
				cErrorMessage::showMessage(
					QObject::tr("Selected file doesn't contain valid gradient of colors"),
					cErrorMessage::errorMessage);
				return;
			}

			file.close();
		}
	}
}

bool cGradientEditWidget::DecodeGradientFromFile(QString string)
{
	QString header = "[gradient]";
	if (string.left(header.length()) == header)
	{
		QString gradientText = string.mid(header.length() + 1);
		gradient.SetColorsFromString(gradientText);
		return true;
	}
	return false;
}

void cGradientEditWidget::SaveColors()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Gradients (*.gradient *.txt)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastGradientFile).absolutePath()));
	dialog.selectFile(
		QDir::toNativeSeparators(QFileInfo(systemData.lastGradientFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save gradient..."));
	dialog.setDefaultSuffix("gradient");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		systemData.lastGradientFile = filename;

		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QString string = gradient.GetColorsAsString();
			QTextStream stream(&file);
			stream << "[gradient] " << string << endl;
			file.close();
		}
	}
}

void cGradientEditWidget::LoadFromClipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString stringLoaded = clipboard->text();

	bool result = DecodeGradientFromFile(stringLoaded);
	if (!result)
	{
		cErrorMessage::showMessage(QObject::tr("Clipboard doesn't contain valid gradient of colors"),
			cErrorMessage::errorMessage);
		return;
	}
}

void cGradientEditWidget::SaveToClipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString string = QString("[gradient] ") + gradient.GetColorsAsString();
	clipboard->setText(string);
}

void cGradientEditWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu();
	QAction *actionAddColor = menu->addAction(tr("Add color"));
	QAction *actionRemoveColor = menu->addAction(tr("Remove color"));
	menu->addSeparator();
	QAction *actionClear = menu->addAction(tr("Delete all colors"));
	QAction *actionChangeNumberOfColors = menu->addAction(tr("Change number of colors ..."));
	QAction *actionGrabColors = menu->addAction(tr("Grab colors from image ..."));
	QAction *actionLoad = menu->addAction(tr("Load colors from file ..."));
	QAction *actionSave = menu->addAction(tr("Save colors to file ..."));
	QAction *actionCopy = menu->addAction(tr("Copy"));
	QAction *actionPaste = menu->addAction(tr("Paste"));

	const QAction *selectedItem = CommonMyWidgetWrapper::contextMenuEvent(event, menu);

	if (selectedItem)
	{
		if (selectedItem == actionAddColor) AddColor(event);
		if (selectedItem == actionRemoveColor) RemoveColor(event);
		if (selectedItem == actionClear) Clear();
		if (selectedItem == actionChangeNumberOfColors) ChangeNumberOfColors();
		if (selectedItem == actionGrabColors) GrabColors();
		if (selectedItem == actionLoad) LoadColors();
		if (selectedItem == actionSave) SaveColors();
		if (selectedItem == actionCopy) SaveToClipboard();
		if (selectedItem == actionPaste) LoadFromClipboard();
	}
}

void cGradientEditWidget::pressedButtonRandomColors()
{
	for (int i = 1; i < gradient.GetNumberOfColors(); i++)
	{
		sRGB color(Random(255), Random(255), Random(255));
		gradient.ModifyColor(i, color);
		if (i == 1) gradient.ModifyColor(0, color);
	}
	update();
}

void cGradientEditWidget::pressedButtonRandomColorsAndPositions()
{
	cRandom random;
	random.Initialize(QTime::currentTime().msec());

	int numberOfColors = random.Random(18) + 2;
	gradient.DeleteAll();
	for (int i = 0; i < numberOfColors; i++)
	{
		sRGB color(random.Random(255), random.Random(255), random.Random(255));
		float position = Random(10000) / 10000.0f;
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
	update();
}

void cGradientEditWidget::pressedButtonBrightnessInc()
{
	for (int i = 1; i < gradient.GetNumberOfColors(); i++)
	{
		sRGB color = gradient.GetColorByIndex(i);
		color.R = qBound(0, int(color.R * 1.2), 255);
		color.G = qBound(0, int(color.G * 1.2), 255);
		color.B = qBound(0, int(color.B * 1.2), 255);
		gradient.ModifyColor(i, color);
		if (i == 1) gradient.ModifyColor(0, color);
	}
	update();
}

void cGradientEditWidget::pressedButtonBrightnessDec()
{

	for (int i = 1; i < gradient.GetNumberOfColors(); i++)
	{
		sRGB color = gradient.GetColorByIndex(i);
		color.R = qBound(0, int(color.R * 0.8), 255);
		color.G = qBound(0, int(color.G * 0.8), 255);
		color.B = qBound(0, int(color.B * 0.8), 255);
		gradient.ModifyColor(i, color);
		if (i == 1) gradient.ModifyColor(0, color);
	}
	update();
}

void cGradientEditWidget::pressedButtonSaturationInc()
{
	for (int i = 1; i < gradient.GetNumberOfColors(); i++)
	{
		sRGB color = gradient.GetColorByIndex(i);
		int average = (color.R + color.G + color.B) / 3;
		color.R = qBound(0, int((color.R - average) * 0.2 + color.R), 255);
		color.G = qBound(0, int((color.G - average) * 0.2 + color.G), 255);
		color.B = qBound(0, int((color.B - average) * 0.2 + color.B), 255);

		gradient.ModifyColor(i, color);
		if (i == 1) gradient.ModifyColor(0, color);
	}
	update();
}

void cGradientEditWidget::pressedButtonSaturationDec()
{
	for (int i = 1; i < gradient.GetNumberOfColors(); i++)
	{
		sRGB color = gradient.GetColorByIndex(i);
		int average = (color.R + color.G + color.B) / 3;
		color.R = qBound(0, int((color.R - average) * -0.2 + color.R), 255);
		color.G = qBound(0, int((color.G - average) * -0.2 + color.G), 255);
		color.B = qBound(0, int((color.B - average) * -0.2 + color.B), 255);

		gradient.ModifyColor(i, color);
		if (i == 1) gradient.ModifyColor(0, color);
	}
	update();
}

void cGradientEditWidget::resetToDefault()
{
	SetColors(defaultValue);
	update();
}

QString cGradientEditWidget::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		defaultValue = parameterContainer->GetDefault<QString>(parameterName);
		setToolTipText();
		gotDefault = true;
	}
	return defaultValue;
}

QString cGradientEditWidget::getDefaultAsString()
{
	return GetColors();
}

QString cGradientEditWidget::getFullParameterName()
{
	return parameterName;
}
