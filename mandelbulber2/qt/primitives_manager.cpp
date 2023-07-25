/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#include "primitives_manager.h"

#include <QInputDialog>
#include "src/error_message.hpp"
#include "src/my_ui_loader.h"
#include "ui_primitives_manager.h"

//#include "primitive_editor.h"
#include "my_tab_bar_with_checkbox.h"

#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/primitives.h"
#include "src/primitive_item.h"
#include "src/render_window.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/settings.hpp"
#include "src/synchronize_interface.hpp"
#include "src/system_directories.hpp"

cPrimitivesManager::cPrimitivesManager(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cPrimitivesManager)
{
	ui->setupUi(this);

	// FIXME:	connect(ui->pushButton_newPrimitive, &QPushButton::clicked, this,
	//		&cPrimitivesManager::slotButtonAddPrimitive);

	SetIconSizes();

	connect(ui->pushButton_add_primitive_box, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_circle, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_cone, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_cylinder, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_plane, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_rectangle, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_sphere, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_torus, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_water, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_add_primitive_prism, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAddPrimitive);

	connect(ui->pushButton_deletePrimitive, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonDeletePrimitive);
	connect(ui->pushButton_duplicatePrimitive, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonDuplicatePrimitive);
	connect(ui->pushButton_placePrimitive, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonPlacePrimitive);
	connect(ui->pushButton_alignRotation, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonAlignRotation);

	connect(ui->pushButton_enableAll, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonEnableAll);
	connect(ui->pushButton_disableAll, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonDisableAll);
	connect(ui->pushButton_onlySelected, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonOnlySelected);

	connect(ui->checkBox_show_wireframe_primitives, &MyCheckBox::stateChanged, this,
		&cPrimitivesManager::slorChangedWireframeVisibikity);

	connect(ui->tabWidget_primitives, &MyTabWidgetWithCheckboxes::currentChanged, this,
		&cPrimitivesManager::slotChangedCurrentTab);

	connect(ui->tabWidget_primitives, &MyTabWidgetWithCheckboxes::contextMenuRequested, this,
		&cPrimitivesManager::slotContextMenu);

	autoRefreshTimer = new QTimer(this);
	autoRefreshTimer->setSingleShot(true);
	connect(autoRefreshTimer, &QTimer::timeout, this, &cPrimitivesManager::slotPeriodicRefresh);
}

void cPrimitivesManager::Init()
{

	Regenerate();

	autoRefreshTimer->start(int(params->Get<double>("auto_refresh_period") * 1000.0));
}

cPrimitivesManager::~cPrimitivesManager()
{
	delete ui;
}

void cPrimitivesManager::AddPrimitive(bool init, const sPrimitiveItem &primitive)
{

	QString primitiveFullName = primitive.fullName;
	QString primitiveType = primitive.typeName;
	QString uiFileName = systemDirectories.sharedDir + "formula" + QDir::separator() + "ui"
											 + QDir::separator() + "primitive_" + primitiveType + ".ui";

	// load ui
	MyUiLoader loader;
	QFile uiFile(uiFileName);
	if (uiFile.exists())
	{
		uiFile.open(QFile::ReadOnly);
		QWidget *newEditor = loader.load(&uiFile);
		uiFile.close();
		int newTabIndex = ui->tabWidget_primitives->count() + 1;

		if (init)
		{
			InitPrimitiveParams(primitive, params);
		}

		// QString name = QString("%1 #%2").arg(primitiveType).arg(newId);
		QString name = params->Get<QString>(primitive.Name("name"));

		ui->tabWidget_primitives->addTab(newEditor, name);

		auto *tabBar = qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_primitives->tabBar());
		tabBar->AddCheckBox(newTabIndex - 1, QString("checkBox_%1_enabled").arg(primitiveFullName));

		if (params->Get<bool>("ui_colorize"))
			cInterface::ColorizeGroupBoxes(ui->tabWidget_primitives->widget(newTabIndex - 1),
				params->Get<int>("ui_colorize_random_seed") + newTabIndex);

		if (init)
		{
			params->Set(primitive.Name("enabled"), true);
		}

		// rename widgets
		QList<QWidget *> listOfWidgets = newEditor->findChildren<QWidget *>();
		for (auto &widget : listOfWidgets)
		{
			QString widgetName = widget->objectName();
			int firstDash = widgetName.indexOf('_');
			QString newName = widgetName.insert(firstDash + 1, primitiveFullName + "_");
			widget->setObjectName(newName);
		}
	}

	else
	{
		cErrorMessage::showMessage(QObject::tr("Can't open file ") + uiFileName
																 + QObject::tr(" Primitive object ui file can't be loaded"),
			cErrorMessage::errorMessage, this);
	}

	primitiveItemOnTab.append(primitive);

	SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::write);
}

void cPrimitivesManager::Regenerate()
{

	// deleting all tabs
	int count = ui->tabWidget_primitives->count();
	for (int i = count - 1; i >= 0; i--)
	{
		delete ui->tabWidget_primitives->widget(i);

		// checkbox is not deleted with tab, so need to be deleted separately
		qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_primitives->tabBar())->RemoveCheckBox(i);

		ui->tabWidget_primitives->removeTab(i);
	}

	primitiveItemOnTab.clear();

	// finding primitives in whole parameter set
	QList<sPrimitiveItem> listOfFoundPrimitives = cPrimitives::GetListOfPrimitives(params);

	// add primitives in sorted order
	std::stable_sort(listOfFoundPrimitives.begin(), listOfFoundPrimitives.end(),
		[](sPrimitiveItem item1, sPrimitiveItem item2) { return item1.id > item2.id; });
	std::stable_sort(listOfFoundPrimitives.begin(), listOfFoundPrimitives.end(),
		[](sPrimitiveItem item1, sPrimitiveItem item2) { return item1.type > item2.type; });
	for (sPrimitiveItem item : listOfFoundPrimitives)
	{
		AddPrimitive(false, item);
	}

	if (mouseFunctionComboWidget) cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);

	if (renderedImageWidget)
	{
		int currentTabIndex = ui->tabWidget_primitives->currentIndex();
		if (currentTabIndex >= 0)
		{
			sPrimitiveItem currentPrimitiveItem = primitiveItemOnTab.at(currentTabIndex);
			renderedImageWidget->SetCurrentPrimitiveItem(currentPrimitiveItem);
			renderedImageWidget->update();
		}
	}

	emit signalUpdatePrimitivesCombos();
}

void cPrimitivesManager::slotButtonAddPrimitive()
{
	QString buttonName = this->sender()->objectName();
	QString primitiveTypeName = buttonName.mid(buttonName.lastIndexOf('_') + 1);

	fractal::enumObjectType objectType = cPrimitives::PrimitiveNameToEnum(primitiveTypeName);
	QList<sPrimitiveItem> actualList = cPrimitives::GetListOfPrimitives(params);

	int newIndex = cPrimitives::NewPrimitiveIndex(primitiveTypeName, actualList);
	QString primitiveFullName = QString("primitive_%1_%2").arg(primitiveTypeName).arg(newIndex);

	sPrimitiveItem newPrimitive(objectType, newIndex, primitiveFullName, primitiveTypeName);

	AddPrimitive(true, newPrimitive);

	params->Set(primitiveFullName + "_enabled", true);

	cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
	ui->tabWidget_primitives->setCurrentIndex(ui->tabWidget_primitives->count() - 1);

	renderedImageWidget->SetCurrentPrimitiveItem(newPrimitive);

	emit signalUpdatePrimitivesCombos();
}

void cPrimitivesManager::slotButtonDuplicatePrimitive()
{
	SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::read);

	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	sPrimitiveItem currentPrimitive = primitiveItemOnTab.at(currentTabIndex);
	QList<sPrimitiveItem> actualList = cPrimitives::GetListOfPrimitives(params);
	int newIndex = cPrimitives::NewPrimitiveIndex(currentPrimitive.typeName, actualList);
	QString primitiveFullName =
		QString("primitive_%1_%2").arg(currentPrimitive.typeName).arg(newIndex);
	sPrimitiveItem newPrimitive(
		currentPrimitive.type, newIndex, primitiveFullName, currentPrimitive.typeName);

	AddPrimitive(true, newPrimitive);

	params->Set(primitiveFullName + "_enabled", true);

	QList<QString> listOfCurrentPrimitiveParams =
		cPrimitives::GetListOfPrimitiveParams(currentPrimitive, params);

	for (const QString &parameterName : listOfCurrentPrimitiveParams)
	{
		QString fullParameterNameSource = parameterName;
		int firstIndexOfDash = parameterName.indexOf('_');
		int secondIndexOfDash = parameterName.indexOf('_', firstIndexOfDash + 1);
		int thirdIndexOfDash = parameterName.indexOf('_', secondIndexOfDash + 1);

		QString subParameterName = parameterName.mid(thirdIndexOfDash + 1);
		if (subParameterName != "name") // to skip coppying "name" parameter
		{
			QString fullParameterNameDest = newPrimitive.Name(subParameterName);

			cOneParameter sourcePar = params->GetAsOneParameter(fullParameterNameSource);
			cMultiVal sourceVar = sourcePar.GetMultiVal(valueActual);

			cOneParameter destPar = params->GetAsOneParameter(fullParameterNameDest);
			destPar.SetMultiVal(sourceVar, valueActual);
			params->SetFromOneParameter(fullParameterNameDest, destPar);
		}
	}

	SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::write);

	cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
	ui->tabWidget_primitives->setCurrentIndex(ui->tabWidget_primitives->count() - 1);

	emit signalUpdatePrimitivesCombos();
}

void cPrimitivesManager::slotButtonDeletePrimitive()
{
	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	if (currentTabIndex >= 0)
	{
		sPrimitiveItem currentPrimitiveIndex = primitiveItemOnTab.at(currentTabIndex);

		QList<QString> listOfPrimitiveParams =
			cPrimitives::GetListOfPrimitiveParams(currentPrimitiveIndex, params);

		for (QString parameterName : listOfPrimitiveParams)
		{
			params->DeleteParameter(parameterName);
		}

		Regenerate();
	}
}

void cPrimitivesManager::slotPeriodicRefresh()
{
	if (ui->checkBox_show_wireframe_primitives->isChecked() && !visibleRegion().isEmpty())
	{
		SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::read);
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(params, nullptr);
		QString newHash = tempSettings.GetHashCode();

		if (newHash != autoRefreshLastHash)
		{
			autoRefreshLastHash = newHash;
			renderedImageWidget->update();
		}
	}

	autoRefreshTimer->start(int(params->Get<double>("auto_refresh_period") * 1000.0));
}

void cPrimitivesManager::slorChangedWireframeVisibikity(int enabled)
{
	renderedImageWidget->SetPrimitivesVisibility(enabled);
	renderedImageWidget->update();
}

void cPrimitivesManager::slotButtonPlacePrimitive()
{
	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	sPrimitiveItem currentPrimitive = primitiveItemOnTab.at(currentTabIndex);

	QList<QVariant> item;

	item.append(int(RenderedImage::clickPlacePrimitive));
	item.append(int(currentPrimitive.type));
	item.append(currentPrimitive.id);
	item.append(currentPrimitive.fullName); // light number

	int index = mouseFunctionComboWidget->findData(item);
	mouseFunctionComboWidget->setCurrentIndex(index);
	renderedImageWidget->setClickMode(item);
}

void cPrimitivesManager::slotChangedCurrentTab(int index)
{
	if (index >= 0 && primitiveItemOnTab.size() > 0)
	{
		sPrimitiveItem currentPrimitiveItem = primitiveItemOnTab.at(index);
		renderedImageWidget->SetCurrentPrimitiveItem(currentPrimitiveItem);
		renderedImageWidget->update();

		RenderedImage::enumClickMode actualClickMode =
			RenderedImage::enumClickMode(mouseFunctionComboWidget->currentData().toList().at(0).toInt());

		if (actualClickMode == RenderedImage::clickPlacePrimitive)
		{
			QList<QVariant> item;
			item.append(int(RenderedImage::clickPlacePrimitive));
			item.append(int(currentPrimitiveItem.type));
			item.append(currentPrimitiveItem.id);
			item.append(currentPrimitiveItem.fullName); // light number

			int index = mouseFunctionComboWidget->findData(item);
			mouseFunctionComboWidget->setCurrentIndex(index);
			renderedImageWidget->setClickMode(item);
		}
	}
}

void cPrimitivesManager::slotButtonAlignRotation()
{
	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	if (currentTabIndex >= 0)
	{
		sPrimitiveItem currentPrimitiveItem = primitiveItemOnTab.at(currentTabIndex);

		SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::read);

		CVector3 camera = gPar->Get<CVector3>("camera");
		CVector3 target = gPar->Get<CVector3>("target");
		CVector3 cameraTopVector = gPar->Get<CVector3>("camera_top");
		cCameraTarget cameraTarget(camera, target, cameraTopVector);
		CVector3 cameraRotation = cameraTarget.GetRotation();

		CVector3 baseX = CVector3(1.0, 0.0, 0.0);
		CVector3 baseY = CVector3(0.0, 1.0, 0.0);
		CVector3 baseZ = CVector3(0.0, 0.0, 1.0);

		// calculation of inverted rotation matrix
		CRotationMatrix mRot;
		mRot.RotateY(-cameraRotation.x); // yaw
		mRot.RotateX(-cameraRotation.y); // pitch
		mRot.RotateZ(-cameraRotation.z); // roll

		baseX = mRot.RotateVector(baseX);
		baseY = mRot.RotateVector(baseY);
		baseZ = mRot.RotateVector(baseZ);

		double alpha = (atan2(baseY.z, baseY.y));
		double beta = -atan2(baseY.x, sqrt(baseY.z * baseY.z + baseY.y * baseY.y));

		CVector3 vectorTemp = baseX;
		vectorTemp = vectorTemp.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), -alpha);
		vectorTemp = vectorTemp.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), -beta);

		double gamma = -atan2(vectorTemp.z, vectorTemp.x);

		alpha = cCameraTarget::CorrectAngle(alpha);
		beta = cCameraTarget::CorrectAngle(beta);
		gamma = cCameraTarget::CorrectAngle(gamma);

		CVector3 rotationAligned(alpha * 180.0 / M_PI, beta * 180.0 / M_PI, gamma * 180.0 / M_PI);

		QString primitiveName = currentPrimitiveItem.fullName;

		gPar->Set(primitiveName + "_rotation", rotationAligned);
		SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::write);
	}
}

void cPrimitivesManager::slotButtonEnableAll()
{
	QList<sPrimitiveItem> listOfFoundPrimitives = cPrimitives::GetListOfPrimitives(params);
	for (const sPrimitiveItem &item : listOfFoundPrimitives)
	{
		params->Set(item.fullName + "_enabled", true);
	}
	SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::write);
}

void cPrimitivesManager::slotButtonDisableAll()
{
	QList<sPrimitiveItem> listOfFoundPrimitives = cPrimitives::GetListOfPrimitives(params);
	for (const sPrimitiveItem &item : listOfFoundPrimitives)
	{
		params->Set(item.fullName + "_enabled", false);
	}
	SynchronizeInterfaceWindow(this, params, qInterface::write);
}

void cPrimitivesManager::slotButtonOnlySelected()
{
	QList<sPrimitiveItem> listOfFoundPrimitives = cPrimitives::GetListOfPrimitives(params);
	for (const sPrimitiveItem &item : listOfFoundPrimitives)
	{
		params->Set(item.fullName + "_enabled", false);
	}

	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	if (currentTabIndex >= 0)
	{
		sPrimitiveItem currentPrimitiveItem = primitiveItemOnTab.at(currentTabIndex);
		params->Set(currentPrimitiveItem.fullName + "_enabled", true);
	}

	SynchronizeInterfaceWindow(this, params, qInterface::write);
}

void cPrimitivesManager::slotContextMenu(const QPoint &screenPoint, int tabIndex)
{
	QMenu menu(this);
	sPrimitiveItem currentPrimitiveItem = primitiveItemOnTab.at(tabIndex);

	QAction *actionRename = menu.addAction(
		tr("Rename %1 #%2").arg(currentPrimitiveItem.typeName).arg(currentPrimitiveItem.id));

	QAction *selectedAction = menu.exec(screenPoint);

	QString oldName = params->Get<QString>(currentPrimitiveItem.fullName + "_name");

	if (selectedAction)
	{
		if (selectedAction == actionRename)
		{
			bool ok = false;
			QString newName = QInputDialog::getText(this,
				tr("Renaming %1 #%2").arg(currentPrimitiveItem.typeName).arg(currentPrimitiveItem.id),
				tr("Enter name of the primitive %1 #%2")
					.arg(currentPrimitiveItem.typeName)
					.arg(currentPrimitiveItem.id),
				QLineEdit::Normal, oldName, &ok);

			if (ok)
			{
				params->Set(currentPrimitiveItem.fullName + "_name", newName);
				ui->tabWidget_primitives->setTabText(tabIndex, newName);
				emit signalUpdatePrimitivesCombos();
			}
		}
	}
}

void cPrimitivesManager::SetIconSizes()
{
	QFontMetrics fm(font());
	int pixelFontSize = fm.height();
	QSize iconSize(int(pixelFontSize * 2.5), int(pixelFontSize * 2.5));
	ui->pushButton_add_primitive_box->setIconSize(iconSize);
	ui->pushButton_add_primitive_circle->setIconSize(iconSize);
	ui->pushButton_add_primitive_cone->setIconSize(iconSize);
	ui->pushButton_add_primitive_cylinder->setIconSize(iconSize);
	ui->pushButton_add_primitive_plane->setIconSize(iconSize);
	ui->pushButton_add_primitive_prism->setIconSize(iconSize);
	ui->pushButton_add_primitive_rectangle->setIconSize(iconSize);
	ui->pushButton_add_primitive_sphere->setIconSize(iconSize);
	ui->pushButton_add_primitive_torus->setIconSize(iconSize);
	ui->pushButton_add_primitive_water->setIconSize(iconSize);
}
