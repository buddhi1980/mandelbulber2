/*
 * multiple_container.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: krzysztof
 */

#include "animation_frames.hpp"

cAnimationFrames::cAnimationFrames()
{
	listPar.clear();
	listFract.clear();
}

void cAnimationFrames::AddFrame(const cParameterContainer &params, const cFractalContainer &fractal)
{
	listPar.append(params);
	listFract.append(fractal);
}

bool cAnimationFrames::GetFrame(int index, cParameterContainer *params, cFractalContainer *fractal)
{
	int count = listPar.count();
	if(index < count)
	{
		*params = listPar.at(index);
		return true;
	}
	else
	{
		*fractal = listFract.at(index);
		return false;
	}
}

int cAnimationFrames::GetNumberOfFrames()
{
	return listPar.count();
}

void cAnimationFrames::Clear()
{
	cAnimationFrames();
}
