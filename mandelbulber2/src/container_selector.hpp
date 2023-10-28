/*
 * container_selector.hpp
 *
 *  Created on: 28 paź 2023
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_CONTAINER_SELECTOR_HPP_
#define MANDELBULBER2_SRC_CONTAINER_SELECTOR_HPP_

#include <QString>
#include <memory>

class cParameterContainer;
class cFractalContainer;

namespace parameterContainer
{
std::shared_ptr<cParameterContainer> ContainerSelector(QString fullParameterName,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal);

// const std::shared_ptr<cParameterContainer> ContainerSelector(QString fullParameterName,
//	const std::shared_ptr<cParameterContainer> params,
//	const std::shared_ptr<cFractalContainer> fractal);
} // namespace parameterContainer

#endif /* MANDELBULBER2_SRC_CONTAINER_SELECTOR_HPP_ */
