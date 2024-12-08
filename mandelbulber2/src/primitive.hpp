/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2022-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * definitions of primitive objects
 */

#ifndef MANDELBULBER2_SRC_PRIMITIVE_HPP_
#define MANDELBULBER2_SRC_PRIMITIVE_HPP_

#include <memory>
#include <vector>

#include "algebra.hpp"
#include "object_data.hpp"
#include "object_types.hpp"
#include "parameters.hpp"

enum enumPrimitiveBooleanOperator
{
	primBooleanOperatorAND = 0,
	primBooleanOperatorOR = 1,
	primBooleanOperatorSUB = 2,
	primBooleanOperatorRevSUB = 3
};

struct sPrimitiveBasic : cObjectData
{
	sPrimitiveBasic(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool enable = false;
	int objectId = 0;
	enumPrimitiveBooleanOperator booleanOperator = primBooleanOperatorOR;
	virtual ~sPrimitiveBasic() = default;
	virtual double PrimitiveDistance(CVector3 _point) const = 0;

	struct sPrimitiveWireLine
	{
		CVector3 p1;
		CVector3 p2;
	};

	typedef std::vector<sPrimitiveWireLine> tWireframeShape;

	static tWireframeShape wireFrameShape;
	static const int wireframeSegments = 24;
	virtual const tWireframeShape &GetWireFrameShape() { return wireFrameShape; }
	static void InitPrimitiveWireframeShapes();
};

struct sPrimitivePlane : sPrimitiveBasic
{
	sPrimitivePlane(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveBox : sPrimitiveBasic
{
	sPrimitiveBox(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	bool limitsEnable;
	double rounding;
	CVector3 repeat;
	CVector3 limitsMax;
	CVector3 limitsMin;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveSphere : sPrimitiveBasic
{
	sPrimitiveSphere(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	bool limitsEnable;
	double radius;
	CVector3 repeat;
	CVector3 limitsMax;
	CVector3 limitsMin;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveCone : sPrimitiveBasic
{
	sPrimitiveCone(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	bool caps;
	bool limitsEnable;
	double radius;
	double height;
	CVector2<double> wallNormal;
	CVector3 repeat;
	CVector3 limitsMax;
	CVector3 limitsMin;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveCylinder : sPrimitiveBasic
{
	sPrimitiveCylinder(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	bool caps;
	bool limitsEnable;
	double radius;
	double height;
	CVector3 repeat;
	CVector3 limitsMax;
	CVector3 limitsMin;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveTorus : sPrimitiveBasic
{
	sPrimitiveTorus(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	bool limitsEnable;
	double radius;
	double radiusLPow;
	double tubeRadius;
	double tubeRadiusLPow;
	CVector3 repeat;
	CVector3 limitsMax;
	CVector3 limitsMin;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveCircle : sPrimitiveBasic
{
	sPrimitiveCircle(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	double radius;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveRectangle : sPrimitiveBasic
{
	sPrimitiveRectangle(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	double height;
	double width;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitivePrism : sPrimitiveBasic
{
	sPrimitivePrism(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	double triangleHeight;
	double height;
	double prismAngle;
	CVector3 normals;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveEllipsoid : sPrimitiveBasic
{
	sPrimitiveEllipsoid(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	bool limitsEnable;
	CVector3 repeat;
	CVector3 limitsMax;
	CVector3 limitsMin;
	double PrimitiveDistance(CVector3 _point) const override;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

struct sPrimitiveWater : sPrimitiveBasic
{
	sPrimitiveWater(const QString &fullName, const std::shared_ptr<cParameterContainer> par);
	bool empty;
	bool waveFromObjectsEnable;
	bool limitsEnable;
	double relativeAmplitude;
	double animSpeed;
	double animProgressionSpeed;
	double length;
	double waveFromObjectsRelativeAmplitude;
	int iterations;
	int animFrame;
	double PrimitiveDistance(CVector3 _point) const override;
	double PrimitiveDistanceWater(CVector3 _point, double distanceFromAnother) const;
	CVector3 limitsMax;
	CVector3 limitsMin;

	static tWireframeShape wireFrameShape;
	static void InitPrimitiveWireframeShape();
	virtual const tWireframeShape &GetWireFrameShape() override { return wireFrameShape; }
};

#endif /* MANDELBULBER2_SRC_PRIMITIVE_HPP_ */
