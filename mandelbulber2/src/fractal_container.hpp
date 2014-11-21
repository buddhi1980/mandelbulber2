/*
 * fractal_container.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: krzysztof
 */

#ifndef SRC_FRACTAL_CONTAINER_HPP_
#define SRC_FRACTAL_CONTAINER_HPP_

#define NUMBER_OF_FRACTALS 4

#include "parameters.hpp"

class cFractalContainer
{
public:
	cParameterContainer &operator[](int index);
	const cParameterContainer &operator[](int index) const;
	cParameterContainer &at(int index);
	const cParameterContainer &at(int index) const;


private:
	cParameterContainer fractals[NUMBER_OF_FRACTALS];
};

#endif /* SRC_FRACTAL_CONTAINER_HPP_ */
