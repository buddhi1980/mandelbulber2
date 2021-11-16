/*
 * my_widget_with_params.cpp
 *
 *  Created on: 16 lis 2021
 *      Author: krzysztof
 */

#include "my_widget_with_params.h"

cMyWidgetWithParams::cMyWidgetWithParams()
{
	// TODO Auto-generated constructor stub
}

void cMyWidgetWithParams::AssignParameterContainers(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	params = _params;
	fractalParams = _fractalParams;
}
