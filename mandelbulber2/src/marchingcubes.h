/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: pmneila
 *
 * From Python Marching Cubes (https://github.com/pmneila/PyMCubes/tree/master/mcubes/src)
 */

#ifndef MANDELBULBER2_SRC_MARCHINGCUBES_H_
#define MANDELBULBER2_SRC_MARCHINGCUBES_H_

#include <stddef.h>
#include <vector>

namespace mc
{

extern int edge_table[256];
extern int triangle_table[256][16];

namespace private_
{

double mc_isovalue_interpolation(double isovalue, double f1, double f2, double x1, double x2);
void mc_add_vertex(double x1, double y1, double z1, double c2, int axis, double f1, double f2,
	double isovalue, std::vector<double> *vertices, double colorIndex_1, double colorIndex_2,
	std::vector<double> *colorIndices);
}

template <typename coord_type, typename vector3, typename formula, typename progressFtor>
void marching_cubes(const vector3 &lower, const vector3 &upper, int numx, int numy, int numz,
	formula f, double isovalue, std::vector<double> &vertices, std::vector<size_t> &polygons,
	bool *stop, progressFtor progress, std::vector<double> &colorIndices)
{
	using namespace private_;

	// typedef decltype(lower[0]) coord_type;

	// numx, numy and numz are the numbers of evaluations in each direction
	--numx;
	--numy;
	--numz;

	coord_type dx = (upper[0] - lower[0]) / static_cast<coord_type>(numx);
	coord_type dy = (upper[1] - lower[1]) / static_cast<coord_type>(numy);
	coord_type dz = (upper[2] - lower[2]) / static_cast<coord_type>(numz);

	size_t *shared_indices = new size_t[numx * numy * numz * 3];
	const int z3 = numz * 3;
	const int yz3 = numy * z3;

	for (int i = 0; i < numx; ++i)
	{
		progress(i);

		coord_type x = lower[0] + dx * i;
		coord_type x_dx = lower[0] + dx * (i + 1);
		for (int j = 0; j < numy; ++j)
		{
			if (*stop)
			{
				progress(-1);
				goto bailout;
			}

			coord_type y = lower[1] + dy * j;
			coord_type y_dy = lower[1] + dy * (j + 1);

			for (int k = 0; k < numz; ++k)
			{
				coord_type z = lower[2] + dz * k;
				coord_type z_dz = lower[2] + dz * (k + 1);

				double v[8];
				double colorIndex[8];
				v[0] = f(x, y, z, &colorIndex[0]);
				v[1] = f(x_dx, y, z, &colorIndex[1]);
				v[2] = f(x_dx, y_dy, z, &colorIndex[2]);
				v[3] = f(x, y_dy, z, &colorIndex[3]);
				v[4] = f(x, y, z_dz, &colorIndex[4]);
				v[5] = f(x_dx, y, z_dz, &colorIndex[5]);
				v[6] = f(x_dx, y_dy, z_dz, &colorIndex[6]);
				v[7] = f(x, y_dy, z_dz, &colorIndex[7]);

				unsigned int cubeindex = 0;
				for (int m = 0; m < 8; ++m)
					if (v[m] <= isovalue) cubeindex |= 1 << m;

				// Generate vertices AVOIDING DUPLICATES.

				int edges = edge_table[cubeindex];
				std::vector<size_t> indices(12, -1);
				if (edges & 0x040)
				{
					indices[6] = vertices.size() / 3;
					shared_indices[i * yz3 + j * z3 + k * 3 + 0] = indices[6];
					mc_add_vertex(x_dx, y_dy, z_dz, x, 0, v[6], v[7], isovalue, &vertices, colorIndex[7],
						colorIndex[7], &colorIndices);
				}
				if (edges & 0x020)
				{
					indices[5] = vertices.size() / 3;
					shared_indices[i * yz3 + j * z3 + k * 3 + 1] = indices[5];
					mc_add_vertex(x_dx, y, z_dz, y_dy, 1, v[5], v[6], isovalue, &vertices, colorIndex[5],
						colorIndex[6], &colorIndices);
				}
				if (edges & 0x400)
				{
					indices[10] = vertices.size() / 3;
					shared_indices[i * yz3 + j * z3 + k * 3 + 2] = indices[10];
					mc_add_vertex(x_dx, y + dx, z, z_dz, 2, v[2], v[6], isovalue, &vertices, colorIndex[2],
						colorIndex[6], &colorIndices);
				}

				if (edges & 0x001)
				{
					if (j == 0 || k == 0)
					{
						indices[0] = vertices.size() / 3;
						mc_add_vertex(x, y, z, x_dx, 0, v[0], v[1], isovalue, &vertices, colorIndex[0],
							colorIndex[1], &colorIndices);
					}
					else
						indices[0] = shared_indices[i * yz3 + (j - 1) * z3 + (k - 1) * 3 + 0];
				}
				if (edges & 0x002)
				{
					if (k == 0)
					{
						indices[1] = vertices.size() / 3;
						mc_add_vertex(x_dx, y, z, y_dy, 1, v[1], v[2], isovalue, &vertices, colorIndex[1],
							colorIndex[2], &colorIndices);
					}
					else
						indices[1] = shared_indices[i * yz3 + j * z3 + (k - 1) * 3 + 1];
				}
				if (edges & 0x004)
				{
					if (k == 0)
					{
						indices[2] = vertices.size() / 3;
						mc_add_vertex(x_dx, y_dy, z, x, 0, v[2], v[3], isovalue, &vertices, colorIndex[2],
							colorIndex[3], &colorIndices);
					}
					else
						indices[2] = shared_indices[i * yz3 + j * z3 + (k - 1) * 3 + 0];
				}
				if (edges & 0x008)
				{
					if (i == 0 || k == 0)
					{
						indices[3] = vertices.size() / 3;
						mc_add_vertex(x, y_dy, z, y, 1, v[3], v[0], isovalue, &vertices, colorIndex[3],
							colorIndex[0], &colorIndices);
					}
					else
						indices[3] = shared_indices[(i - 1) * yz3 + j * z3 + (k - 1) * 3 + 1];
				}
				if (edges & 0x010)
				{
					if (j == 0)
					{
						indices[4] = vertices.size() / 3;
						mc_add_vertex(x, y, z_dz, x_dx, 0, v[4], v[5], isovalue, &vertices, colorIndex[4],
							colorIndex[5], &colorIndices);
					}
					else
						indices[4] = shared_indices[i * yz3 + (j - 1) * z3 + k * 3 + 0];
				}
				if (edges & 0x080)
				{
					if (i == 0)
					{
						indices[7] = vertices.size() / 3;
						mc_add_vertex(x, y_dy, z_dz, y, 1, v[7], v[4], isovalue, &vertices, colorIndex[7],
							colorIndex[4], &colorIndices);
					}
					else
						indices[7] = shared_indices[(i - 1) * yz3 + j * z3 + k * 3 + 1];
				}
				if (edges & 0x100)
				{
					if (i == 0 || j == 0)
					{
						indices[8] = vertices.size() / 3;
						mc_add_vertex(x, y, z, z_dz, 2, v[0], v[4], isovalue, &vertices, colorIndex[0],
							colorIndex[4], &colorIndices);
					}
					else
						indices[8] = shared_indices[(i - 1) * yz3 + (j - 1) * z3 + k * 3 + 2];
				}
				if (edges & 0x200)
				{
					if (j == 0)
					{
						indices[9] = vertices.size() / 3;
						mc_add_vertex(x_dx, y, z, z_dz, 2, v[1], v[5], isovalue, &vertices, colorIndex[1],
							colorIndex[3], &colorIndices);
					}
					else
						indices[9] = shared_indices[i * yz3 + (j - 1) * z3 + k * 3 + 2];
				}
				if (edges & 0x800)
				{
					if (i == 0)
					{
						indices[11] = vertices.size() / 3;
						mc_add_vertex(x, y_dy, z, z_dz, 2, v[3], v[7], isovalue, &vertices, colorIndex[3],
							colorIndex[7], &colorIndices);
					}
					else
						indices[11] = shared_indices[(i - 1) * yz3 + j * z3 + k * 3 + 2];
				}

				int tri;
				int *triangle_table_ptr = triangle_table[cubeindex];
				for (int m = 0; tri = triangle_table_ptr[m], tri != -1; ++m)
					polygons.push_back(indices[tri]);
			}
		}
	}

bailout:

	delete[] shared_indices;
}
}

#endif /* MANDELBULBER2_SRC_MARCHINGCUBES_H_ */
