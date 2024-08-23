/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cLights class - container for light source definitions
 */

#ifndef MANDELBULBER2_SRC_LIGHTS_HPP_
#define MANDELBULBER2_SRC_LIGHTS_HPP_

#include <memory>
#include <vector>

#include <QObject>

#include "algebra.hpp"
#include "color_structures.hpp"
#include "light.h"

// forward declarations
class cParameterContainer;
class cFractalContainer;

// struct sLight
//{
//	CVector3 position;
//	sRGBFloat colour;
//	float intensity{0.0f};
//	bool enabled{false};
//	sLight() {}
//};

class cLights : public QObject
{
	Q_OBJECT
public:
	cLights();
	cLights(const std::shared_ptr<cParameterContainer> _params,
		const std::shared_ptr<cFractalContainer> _fractal, bool loadTextures, bool quiet,
		bool useNetRender);
	void Set(const std::shared_ptr<cParameterContainer> _params,
		const std::shared_ptr<cFractalContainer> _fractal, bool loadTextures, bool quiet,
		bool useNetRender);
	~cLights() override;
	const cLight *GetLight(const int index) const;
	int GetNumberOfLights() const { return numberOfLights; }
	int IsAnyLightEnabled() const { return isAnyLight; };
	static QList<int> GetListOfLights(std::shared_ptr<cParameterContainer> params);

private:
	enum class enumRandomLightsColoringType
	{
		random = 0,
		single = 1,
		two = 2,
		distance = 3
	};

	void Copy(const cLights &);

	std::vector<cLight> lights;
	cLight dummyLight;
	int numberOfLights;
	bool lightsReady;
	bool isAnyLight;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

#endif /* MANDELBULBER2_SRC_LIGHTS_HPP_ */
