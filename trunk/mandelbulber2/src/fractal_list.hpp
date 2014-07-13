/*
 * fractal_list.hpp
 *
 *  Created on: Apr 16, 2014
 *      Author: krzysztof
 */

#ifndef FRACTAL_LIST_HPP_
#define FRACTAL_LIST_HPP_

#include <QtCore>

namespace fractal
{
enum enumFractalFormula
{
	none = 0,
	fast_mandelbulb = 1,
	mandelbulb = 2,
	fast_mandelbulb_power2 = 3,
	hypercomplex = 4,
	quaternion = 5,
	minus_fast_trig = 6,
	menger_sponge = 7,
	mandelbox = 8,
	kaleidoscopic = 10,
	xenodreambuie = 11,
	hybrid = 12,
	mandelbulb2 = 13,
	mandelbulb3 = 14,
	mandelbulb4 = 15,
	boxFoldBulbPow2 = 16,
	smoothMandelbox = 17,
	mandelboxVaryScale4D = 18,
	aexion = 19,
	benesi = 20,
	bristorbrot = 21,
	invertX = 22,
	invertY = 23,
	invertZ = 24,
	invertR = 25,
	sphericalFold = 26,
	powXYZ = 27,
	scaleX = 28,
	scaleY = 29,
	scaleZ = 30,
	offsetX = 31,
	offsetY = 32,
	offsetZ = 33,
	angleMultiplyX = 34,
	angleMultiplyY = 35,
	angleMultiplyZ = 36,
	generalizedFoldBox = 37,
	ocl_custom = 38
};

enum enumDEType
{
	analitycDE, deltaDE
};

}

struct sFractalDescription
{
	QString nameInComboBox;
	QString internalNane;
	int internalID;
	fractal::enumDEType DEType;
	sFractalDescription(QString _nameInComboBox, QString _internalNane, fractal::enumFractalFormula _internalID, fractal::enumDEType _DEType) : nameInComboBox(_nameInComboBox), internalNane(_internalNane), internalID(_internalID), DEType(_DEType) {};
};

extern QList<sFractalDescription> fractalList;

void DefineFractalList(QList<sFractalDescription> *fractalList);

#endif /* FRACTAL_LIST_HPP_ */
