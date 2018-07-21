/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: pmneila, Sebastian Jennen (jenzebas@gmail.com)
 *
 * Re-licensed with the written authorization of P. M. Neila
 * for use with Mandelbulber. See here for original code:
 * https://github.com/pmneila/PyMCubes/
 */

#ifndef MANDELBULBER2_SRC_MARCHINGCUBES_H_
#define MANDELBULBER2_SRC_MARCHINGCUBES_H_

#include <cstddef>
#include <vector>

#include <QObject>

#include "algebra.hpp"

struct sParamRender;
class cNineFractals;
struct sRenderData;
class cParameterContainer;
class cFractalContainer;

class MarchingCubes : public QObject
{
	Q_OBJECT

public:
	MarchingCubes(const cParameterContainer *paramsContainer,
		const cFractalContainer *fractalContainer, sParamRender *params, cNineFractals *fractals,
		sRenderData *renderData, int numx, int numy, int numz, const CVector3 &lower,
		const CVector3 &upper, double dist_thresh, bool *stop, std::vector<double> &vertices,
		std::vector<long long> &polygons, std::vector<double> &colorIndices);

	~MarchingCubes() override { FreeBuffers(); }

public slots:
	void RunMarchingCube();

private:
	void FreeBuffers();
	static int edge_table[256];
	static int triangle_table[256][16];

#ifdef USE_OFFLOAD
	__declspec(target(mic))
#endif // USE_OFFLOAD
		long long *shared_indices;

#ifdef USE_OFFLOAD
	__declspec(target(mic))
#endif // USE_OFFLOAD
		double *voxelBuffer;

#ifdef USE_OFFLOAD
	__declspec(target(mic))
#endif // USE_OFFLOAD
		double *colorBuffer;

	int numx;
	int numy;
	int numz;
	double dx;
	double dy;
	double dz;
	CVector3 lower;
	CVector3 upper;
	long long numyb;
	long long numzb;
	long long numyzb;
	int z3;
	int yz3;
	sParamRender *params;
	cNineFractals *fractals;
	sRenderData *renderData;
	double dist_thresh;
	const cParameterContainer *paramsContainer;
	const cFractalContainer *fractalContainer;
	bool coloredMesh;

	bool *stop;
	std::vector<double> &vertices;
	std::vector<long long> &polygons;
	std::vector<double> &colorIndices;

	void calculateVoxelPlane(int i);

	void calculateEdges(int i);

	double getDistance(double x, double y, double z, double *colorIndex) const;

	inline double mc_isovalue_interpolation(
		double isovalue, double f1, double f2, double x1, double x2)
	{
		if (f2 == f1) return (x2 + x1) / 2;
		return (x2 - x1) * (isovalue - f1) / (f2 - f1) + x1;
	}

	inline void mc_add_vertex(double x1, double y1, double z1, double c2, int axis, double f1,
		double f2, double isovalue, std::vector<double> *vertices, double colorIndex_1,
		double colorIndex_2, std::vector<double> *colorIndices)
	{
		switch (axis)
		{
			case 0:
			{
				double x = mc_isovalue_interpolation(isovalue, f1, f2, x1, c2);
				vertices->push_back(x);
				vertices->push_back(y1);
				vertices->push_back(z1);

				// double f = (x - x1) / (c2 - x1);
				double colorIndex = mc_isovalue_interpolation(isovalue, f1, f2, colorIndex_1,
					colorIndex_2); // f * colorIndex_2 + (1.0 - f) * colorIndex_1;
				colorIndices->push_back(colorIndex);

				return;
			}
			case 1:
			{
				double y = mc_isovalue_interpolation(isovalue, f1, f2, y1, c2);
				vertices->push_back(x1);
				vertices->push_back(y);
				vertices->push_back(z1);

				// double f = (y - y1) / (c2 - y1);
				double colorIndex = mc_isovalue_interpolation(isovalue, f1, f2, colorIndex_1,
					colorIndex_2); // f * colorIndex_2 + (1.0 - f) * colorIndex_1;
				colorIndices->push_back(colorIndex);

				return;
			}
			case 2:
			{
				double z = mc_isovalue_interpolation(isovalue, f1, f2, z1, c2);
				vertices->push_back(x1);
				vertices->push_back(y1);
				vertices->push_back(z);

				// double f = (z - z1) / (c2 - z1);
				double colorIndex = mc_isovalue_interpolation(isovalue, f1, f2, colorIndex_1,
					colorIndex_2); // f * colorIndex_2 + (1.0 - f) * colorIndex_1;
				colorIndices->push_back(colorIndex);

				return;
			}
		}
	}

signals:
	int updateProgressAndStatus(int i);
	void finished();
};

#endif /* MANDELBULBER2_SRC_MARCHINGCUBES_H_ */
