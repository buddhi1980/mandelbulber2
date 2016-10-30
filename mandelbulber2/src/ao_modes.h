/*
 * ao_modes.h
 *
 *  Created on: 30 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_AO_MODES_H_
#define MANDELBULBER2_SRC_AO_MODES_H_


namespace params
{
enum enumAOMode
{
	AOmodeFast = 0,
	AOmodeMultipeRays = 1,
	AOmodeScreenSpace = 2
};
}


#endif /* MANDELBULBER2_SRC_AO_MODES_H_ */
