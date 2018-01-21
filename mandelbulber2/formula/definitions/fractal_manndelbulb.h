/*
 * cFractalMandelbulb.h
 *
 *  Created on: 21 sty 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_FORMULA_DEFINITIONS_FRACTAL_MANNDELBULB_H_
#define MANDELBULBER2_FORMULA_DEFINITIONS_FRACTAL_MANNDELBULB_H_

#include "abstract_fractal.h"

namespace fractal
{

class cFractalMandelbulb : public cAbstractFractal
{
	/**
	 * Classic Mandelbulb fractal.
	 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
	 */

public:
	~cFractalMandelbulb();
	cFractalMandelbulb()
	{
		nameInComboBox = "Mandelbulb";
		internalName = "mandelbulb";
		// internalID = 2;
		DEType = analyticDEType;
		DEFunctionType = logarithmicDEFunction;
		cpixelAddition = cpixelEnabledByDefault;
		defaultBailout = 10.0;
		DEAnalyticFunction = analyticFunctionLogarithmic;
		coloringFunction = coloringFunctionDefault;
	}

	void FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux) override
	{
		// if (aux.r < 1e-21) aux.r = 1e-21;
		const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
		const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
		double rp = pow(aux.r, fractal->bulb.power - 1.0);
		const double th = th0 * fractal->bulb.power;
		const double ph = ph0 * fractal->bulb.power;
		const double cth = cos(th);
		aux.r_dz = (rp * aux.r_dz) * fractal->bulb.power + 1.0;
		rp *= aux.r;
		z.x = cth * cos(ph) * rp;
		z.y = cth * sin(ph) * rp;
		z.z = sin(th) * rp;
	}
};

} /* namespace fractal */

#endif /* MANDELBULBER2_FORMULA_DEFINITIONS_FRACTAL_MANNDELBULB_H_ */
