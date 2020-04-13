/*
 * abstract_fractal.h
 *
 *  Created on: 21 sty 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_
#define MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <QDir>
#include "all_fractal_list_enums.hpp"
#include "src/algebra.hpp"
#include "src/fractal.h"
#include "src/fractal_enums.h"
#include "src/common_math.h"
#include "src/system_directories.hpp"

using std::max;
using std::min;
using std::swap;

// formula constants
#ifndef SQRT_1_3
#define SQRT_1_3 0.57735026918962576450914878050196
#define SQRT_1_2 0.70710678118654752440084436210485
#define SQRT_2_3 0.81649658092772603273242802490196
#define SQRT_3_2 1.22474487139158904909864203735295
#define SQRT_3_4 0.86602540378443864676372317075294
#define SQRT_3_4d2 0.43301270189221932338186158537647
#define SQRT_3 1.73205080756887729352744634150587
#define FRAC_1_3 0.33333333333333333333333333333333
#endif

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576923690768489
#endif

#ifndef M_PI_8
#define M_PI_8 0.39269908169872415480783042290994
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830961566084581988
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923132169163975
#endif

#ifndef M_PI_2x
#define M_PI_2x 6.283185307179586476925286766559
#endif

using namespace fractal;

#define FRACTAL_CLASS(fractalName)                                                      \
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
	virtual void FormulaCode(CVector4 &, const sFractal *, sExtendedAux &) = 0;

protected:
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
	fractal::enumFractalFormula getInternalId() const { return internalID; }
	const QString &getInternalName() const { return internalName; }
	const QString &getNameInComboBox() const { return nameInComboBox; }

	QString getIconName() const
	{
		if (internalID == fractal::none)
		{
			return QString(":system/icons/list-remove.svg");
		}
		return systemDirectories.sharedDir + "formula" + QDir::separator() + "img" + QDir::separator()
					 + internalName + ".png";
	}
	QString getUiFilename() const
	{
		return systemDirectories.sharedDir + "formula" + QDir::separator() + "ui" + QDir::separator()
					 + internalName + ".ui";
	}
	QString getOpenCLFilename() const
	{
		return systemDirectories.sharedDir + "formula" + QDir::separator() + "opencl"
					 + QDir::separator() + internalName + ".cl";
	}
};

#endif /* MANDELBULBER2_FORMULA_DEFINITIONS_ABSTRACT_FRACTAL_H_ */
