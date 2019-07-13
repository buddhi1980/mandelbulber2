/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * output queue for multi-gpu rendering
 */

#ifndef MANDELBULBER2_SRC_OPENCL_WORKER_OUTPUT_QUEUE_H_
#define MANDELBULBER2_SRC_OPENCL_WORKER_OUTPUT_QUEUE_H_

#include <QtCore>

#ifdef USE_OPENCL
class cOpenCLWorkerOutputQueue
{
public:
	struct sClDataBuffer
	{
		sClDataBuffer(quint64 itemSize, quint64 length) : itemSize(itemSize), length(length)
		{
			data.reserve(itemSize * length);
		}

		quint64 size() const { return itemSize * length; }
		quint64 itemSize;
		quint64 length;
		std::vector<char> data;
	};

	struct sClSingleOutput
	{
		quint64 jobX;
		quint64 jobY;
		quint64 jobWidth;
		quint64 jobHeight;
		quint64 gridX;
		quint64 gridY;
		quint64 tileIndex;
		int monteCarloLoop;
		QList<sClDataBuffer> outputBuffers;
	};

	cOpenCLWorkerOutputQueue();
	~cOpenCLWorkerOutputQueue();
	void AddToQueue(const sClSingleOutput *data);
	sClSingleOutput GetFromQueue();
	bool isEmpty();
	int getQueueLength();

private:
	QQueue<sClSingleOutput> queue;
	QMutex lock;
};
#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_WORKER_OUTPUT_QUEUE_H_ */
