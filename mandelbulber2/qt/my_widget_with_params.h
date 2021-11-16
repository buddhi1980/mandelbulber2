/*
 * my_widget_with_params.h
 *
 *  Created on: 16 lis 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MY_WIDGET_WITH_PARAMS_H_
#define MANDELBULBER2_QT_MY_WIDGET_WITH_PARAMS_H_

#include <memory>

class cParameterContainer;
class cFractalContainer;

class cMyWidgetWithParams
{
public:
	cMyWidgetWithParams();
	~cMyWidgetWithParams() = default;

	virtual void AssignParameterContainers(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams);

protected:
	std::shared_ptr<cParameterContainer> params;
	std::shared_ptr<cFractalContainer> fractalParams;
};

#endif /* MANDELBULBER2_QT_MY_WIDGET_WITH_PARAMS_H_ */
