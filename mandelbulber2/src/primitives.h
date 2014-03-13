/*********************************************************
 /                   MANDELBULBER
 / functions for calculating shapes of primitive objects
 /
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "algebra.hpp"

double PrimitivePlane(CVector3 point, CVector3 centre, CVector3 normal);
double PrimitiveInvertedBox(CVector3 point, CVector3 center, CVector3 size);
double PrimitiveBox(CVector3 point, CVector3 center, CVector3 size);
double PrimitiveSphere(CVector3 point, CVector3 center, double radius);
double PrimitiveInvertedSphere(CVector3 point, CVector3 center, double radius);
double PrimitiveWater(CVector3 point, double height, double amplitude, double length, double rotation, int iterations, double animSpeed, int frame);

#endif /* PRIMITIVES_HPP_ */
