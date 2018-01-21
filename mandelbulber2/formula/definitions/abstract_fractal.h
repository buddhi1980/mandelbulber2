/*
 * abstract_fractal.h
 *
 *  Created on: 21 sty 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_
#define MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_

#include <QtCore>
#include "fractal_enums.h"
#include "algebra.hpp"
#include "fractal.h"

namespace fractal
{

typedef void (*fractalFormulaFcn)(CVector4 &, const sFractal *, sExtendedAux &);

class cAbstractFractal
{
public:
	cAbstractFractal();
	virtual ~cAbstractFractal();

public:
	virtual QString nameInComboBox;
	virtual QString internalName;
	// fractal::enumFractalFormula internalID; //FIXME there is needed nice solution to provide enums
	fractalFormulaFcn fractalFormulaFunction;
	fractal::enumDEType DEType;
	fractal::enumDEFunctionType DEFunctionType;
	fractal::enumCPixelAddition cpixelAddition;
	double defaultBailout;
	fractal::enumDEAnalyticFunction DEAnalyticFunction;
	fractal::enumColoringFunction coloringFunction;

	virtual void FormulaCode(CVector4 &, const sFractal *, sExtendedAux &);
	bool CheckForErrors(); // this method will be used in NineFractals class
};

} /* namespace fractal */

#endif /* MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_ */
