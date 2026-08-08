/*
 * abstract_fractal.cpp
 *
 *  Created on: 21 sty 2018
 *      Author: krzysztof
 */

#include <QDebug>
#include <QDir>
#include "abstract_fractal.h"
#include "src/system_directories.hpp"

cAbstractFractal::cAbstractFractal()
{
	nameInComboBox = "";
	internalName = "";
	internalID = invalid;
	DEType = undefinedDEType;
	DEFunctionType = undefinedDEFunction;
	cpixelAddition = cpixelUndefined;
	defaultBailout = -1.0;
	DEAnalyticFunction = analyticFunctionUndefined;
	coloringFunction = coloringFunctionUndefined;
}

cAbstractFractal::~cAbstractFractal() {}

bool cAbstractFractal::CheckForErrors() const
{
	bool error = false;
	if (nameInComboBox == "")
	{
		qCritical() << "Undefined nameInComboBox in formula " << internalName;
		error = true;
	}
	if (internalName == "")
	{
		qCritical() << "Undefined internalName in formula " << nameInComboBox;
		error = true;
	}
	if (internalID == -1)
	{
		qCritical() << "Undefined internalID in formula " << internalName;
		error = true;
	}
	if (DEType == undefinedDEType)
	{
		qCritical() << "Undefined DEType in formula " << internalName;
		error = true;
	}
	if (DEFunctionType == undefinedDEFunction)
	{
		qCritical() << "Undefined DEFunctionType in formula " << internalName;
		error = true;
	}
	if (cpixelAddition == cpixelUndefined)
	{
		qCritical() << "Undefined cpixelAddition in formula " << internalName;
		error = true;
	}
	if (defaultBailout == -1.0)
	{
		qCritical() << "Undefined defaultBailout in formula " << internalName;
		error = true;
	}
	if (DEAnalyticFunction == analyticFunctionUndefined)
	{
		qCritical() << "Undefined DEAnalyticFunction in formula " << internalName;
		error = true;
	}
	if (coloringFunction == coloringFunctionUndefined)
	{
		qCritical() << "Undefined coloringFunction in formula " << internalName;
		error = true;
	}

	return error;
}

QString cAbstractFractal::getIconName() const
{
	if (internalID == fractal::none)
	{
		return QString(":system/icons/list-remove.svg");
	}
	return systemDirectories.sharedDir + "formula" + QDir::separator() + "img" + QDir::separator()
				 + internalName + ".png";
}

QString cAbstractFractal::getUiFilename() const
{
	return systemDirectories.sharedDir + "formula" + QDir::separator() + "ui" + QDir::separator()
				 + internalName + ".ui";
}

QString cAbstractFractal::getOpenCLFilename() const
{
	return systemDirectories.sharedDir + "formula" + QDir::separator() + "opencl" + QDir::separator()
				 + internalName + ".cl";
}
