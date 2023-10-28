/*
 * container_selector.cpp
 *
 *  Created on: 28 paź 2023
 *      Author: krzysztof
 */

#include "container_selector.hpp"

#include "fractal_container.hpp"
#include "parameters.hpp"
#include <QDebug>

namespace parameterContainer
{

std::shared_ptr<cParameterContainer> ContainerSelector(QString fullParameterName,
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
		const int index = containerName.right(1).toInt();
		if (index < NUMBER_OF_FRACTALS)
		{
			container = fractal->at(index);
		}
		else
		{
			qWarning() << "ContainerSelector(): wrong fractal container index" << containerName << index;
		}
	}
	else
	{
		qWarning() << "ContainerSelector(): wrong container name" << containerName;
	}

	return container;
}
} // namespace parameterContainer
