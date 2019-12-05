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
#include "src/fractal_list_enums.hpp"
#include "src/algebra.hpp"
#include "src/fractal.h"

using namespace fractal;

#define FRACTAL_CLASS(fractalName)                                                         \
	class fractalName : public cAbstractFractal                                           \
	{                                                                                     \
	public:                                                                               \
		fractalName();                                                                      \
		void FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux) override; \
	};

class cAbstractFractal
{
public:
	cAbstractFractal();
	virtual ~cAbstractFractal();

public:
	bool CheckForErrors() const; // this method will be used in NineFractals class

protected:
	virtual void FormulaCode(CVector4 &, const sFractal *, sExtendedAux &) = 0;
	QString nameInComboBox;
	QString internalName;
	fractal::enumFractalFormula internalID;
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
	const QString &getInternalName() const { return internalName; }
	const QString &getNameInComboBox() const { return nameInComboBox; }
};

#endif /* MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_ */
