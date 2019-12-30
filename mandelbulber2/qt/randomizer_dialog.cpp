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

void cRandomizerDialog::Randomize(enimRandomizeStrength sthength)
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
		QString widgetName = QString("previewwidget_%1").arg(i + 1, 2, 10, QChar('0'));
		qDebug() << widgetName;
		cThumbnailWidget *previewWidget = this->findChild<cThumbnailWidget *>(widgetName);
		qDebug() << previewWidget;
		previewWidget->SetSize(192, 128, 2);
		previewWidget->AssignParameters(listOfVersions.at(i).params, listOfVersions.at(i).fractParams);
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
