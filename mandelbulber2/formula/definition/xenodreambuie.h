/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2018 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Xenodreambuie
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/273/
 */

#ifndef MANDELBULBER2_FORMULA_DEFINITION_FRACTAL_XENODREAMBUIE_H_
#define MANDELBULBER2_FORMULA_DEFINITION_FRACTAL_XENODREAMBUIE_H_

#include "abstract_fractal.h"

namespace fractal
{

class cFractalXenodreambuie : public cAbstractFractal
{
public:
	~cFractalXenodreambuie();
	cFractalXenodreambuie()
	{
		nameInComboBox = "Xenodreambuie";
		internalName = "xenodreambuie";
		internalID = 11;
		DEType = analyticDEType;
		DEFunctionType = logarithmicDEFunction;
		cpixelAddition = cpixelEnabledByDefault;
		defaultBailout = 10.0;
		DEAnalyticFunction = analyticFunctionLogarithmic;
		coloringFunction = coloringFunctionDefault;
	}

	void FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux) override
	{
		double rp = pow(aux.r, fractal->bulb.power - 1.0);
		aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
		rp *= aux.r;

		double th = atan2(z.y, z.x) + fractal->bulb.betaAngleOffset;
		double ph = acos(z.z / aux.r) + fractal->bulb.alphaAngleOffset;

		if (fabs(ph) > 0.5 * M_PI) ph = sign(ph) * M_PI - ph;

		z.x = rp * cos(th * fractal->bulb.power) * sin(ph * fractal->bulb.power);
		z.y = rp * sin(th * fractal->bulb.power) * sin(ph * fractal->bulb.power);
		z.z = rp * cos(ph * fractal->bulb.power);
	}
};

} /* namespace fractal */

#endif /* MANDELBULBER2_FORMULA_DEFINITION_FRACTAL_XENODREAMBUIE_H_ */
