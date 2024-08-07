/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * scheduler for multi-gpu rendering
 */

#ifndef MANDELBULBER2_SRC_OPENCL_SCHEDULER_H_
#define MANDELBULBER2_SRC_OPENCL_SCHEDULER_H_

#include <QList>
#include <QMutex>
#include <QPoint>

class cOpenClScheduler
{
	struct sTileStatus
	{
		bool enabled;
		int reserved; // MC iteration number where was reserved
		int done;			// MC iteration number where was done
	};

public:
	cOpenClScheduler(const QList<QPoint> *tileSequence);
	~cOpenClScheduler();
	void EnableAllTiles();
	void DisableTile(int tileIndex);
	void ReserveTile(int tileIndex, int monteCarloIteration);
	bool IsTileEnabled(int tileIndex) { return tiles[tileIndex].enabled; }
	void Clear();
	int GetNextTileToRender(int lastTile, int monteCarloIteration);
	bool AllDone(int monteCarloIteration);

	const QList<QPoint> *getTileSequence() const { return tileSequence; }

private:
	const QList<QPoint> *tileSequence;
	QList<sTileStatus> tiles;

	QMutex lock;
};

#endif /* MANDELBULBER2_SRC_OPENCL_SCHEDULER_H_ */
