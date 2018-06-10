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
	bool CheckForErrors() const; // this method will be used in NineFractals class

protected:
	virtual void FormulaCode(CVector4 &, const sFractal *, sExtendedAux &);
	QString nameInComboBox;
	QString internalName;
	fractal::enumFractalFormula internalID;
	fractalFormulaFcn fractalFormulaFunction;
	fractal::enumDEType DEType;
	fractal::enumDEFunctionType DEFunctionType;
	fractal::enumCPixelAddition cpixelAddition;
	double defaultBailout;
	fractal::enumDEAnalyticFunction DEAnalyticFunction;
	fractal::enumColoringFunction coloringFunction;

public:
	fractal::enumColoringFunction getColoringFunction() const { return coloringFunction; }
	fractal::enumCPixelAddition getCpixelAddition() const { return cpixelAddition; }
	fractal::enumDEAnalyticFunction getDeAnalyticFunction() const { return DEAnalyticFunction; }
	double getDefaultBailout() const { return defaultBailout; }
	fractal::enumDEFunctionType getDeFunctionType() const { return DEFunctionType; }
	fractal::enumDEType getDeType() const { return DEType; }
	fractalFormulaFcn getFractalFormulaFunction() const { return fractalFormulaFunction; }
	const QString &getInternalName() const { return internalName; }
	const QString &getNameInComboBox() const { return nameInComboBox; }
};

} /* namespace fractal */

#endif /* MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_ */
