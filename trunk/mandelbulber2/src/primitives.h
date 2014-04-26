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
#include "color_structures.hpp"

struct sPrimitivePlane
{
	bool enable;
	bool onlyPlane;
	CVector3 centre;
	CVector3 normal;
	double reflect;
	sRGB colour;
};

struct sPrimitiveBox
{
	bool enable;
	CVector3 centre;
	CVector3 size;
	double reflect;
	sRGB colour;
};

struct sPrimitiveSphere
{
	bool enable;
	CVector3 centre;
	double radius;
	double reflect;
	sRGB colour;
};

struct sPrimitiveWater
{
	bool enable;
	double amplitude;
	double animSpeed;
	double level;
	double length;
	double reflect;
	double rotation;
	int iterations;
	sRGB colour;
};

struct sPrimitives
{
	sPrimitivePlane plane;
	sPrimitiveBox box;
	sPrimitiveBox invertedBox;
	sPrimitiveSphere sphere;
	sPrimitiveSphere invertedSphere;
	sPrimitiveWater water;
};

double PrimitivePlane(CVector3 point, CVector3 centre, CVector3 normal);
double PrimitiveInvertedBox(CVector3 point, CVector3 center, CVector3 size);
double PrimitiveBox(CVector3 point, CVector3 center, CVector3 size);
double PrimitiveSphere(CVector3 point, CVector3 center, double radius);
double PrimitiveInvertedSphere(CVector3 point, CVector3 center, double radius);
double PrimitiveWater(CVector3 point, double height, double amplitude, double length, double rotation, int iterations, double animSpeed, int frame);

#endif /* PRIMITIVES_HPP_ */
