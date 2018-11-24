/*
 * abstract_fractal.h
 *
 *  Created on: 21 sty 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_
#define MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_

#include <QtCore>
#include "src/fractal_enums.h"
#include "src/algebra.hpp"
#include "src/fractal.h"

namespace fractal
{

typedef void (*fractalFormulaFcn)(CVector4 &, const sFractal *, sExtendedAux &);

class cAbstractFractal
{
protected:
	virtual const void FormulaCode(CVector4 &, const sFractal *, sExtendedAux &) = 0;

public:
	virtual const fractal::enumColoringFunction getColoringFunction() = 0;
	virtual const fractal::enumCPixelAddition getCpixelAddition() = 0;
	virtual const fractal::enumDEAnalyticFunction getDeAnalyticFunction() = 0;
	virtual const double getDefaultBailout() = 0;
	virtual const fractal::enumDEFunctionType getDeFunctionType() = 0;
	virtual const fractal::enumDEType getDeType() = 0;
	virtual const fractalFormulaFcn getFractalFormulaFunction() = 0;
	virtual const QString &getInternalName() = 0;
	virtual const QString &getNameInComboBox() = 0;
};

} /* namespace fractal */

#endif /* MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_ */
