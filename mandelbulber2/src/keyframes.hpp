/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * Class to store settings for keyframe animation
 *
 * Handles the 2D matrix of the list of parameters / list of frames
 * and exposes functions to modify this matrix. This functionality is
 * derived from the cAnimationFrames class. Additionally this class
 * interpolates sub-frames with the help of the cMorph class.
 */

#ifndef MANDELBULBER2_SRC_KEYFRAMES_HPP_
#define MANDELBULBER2_SRC_KEYFRAMES_HPP_

#include "animation_frames.hpp"
#include "morph.hpp"

class cKeyframes : public cAnimationFrames
{
	// definitions:
	// frame means animation keyframe
	// interpolatedFrame means animation frame

public:
	cKeyframes();
	~cKeyframes() override;
	cKeyframes(const cKeyframes &source);
	cKeyframes &operator=(const cKeyframes &source);
	sAnimationFrame GetInterpolatedFrame(
		int index, cParameterContainer *params, cFractalContainer *fractal);
	void GetInterpolatedFrameAndConsolidate(
		int index, cParameterContainer *params, cFractalContainer *fractal);
	void SetFramesPerKeyframe(int frPerKey) { framesPerKeyframe = frPerKey; }
	int GetFramesPerKeyframe() const { return framesPerKeyframe; }
	void ChangeMorphType(int parameterIndex, parameterContainer::enumMorphType morphType);
	void ClearMorphCache() { morph.clear(); }
	void AddAnimatedParameter(const QString &parameterName, const cOneParameter &defaultValue,
		cParameterContainer *params = nullptr) override;
	bool AddAnimatedParameter(const QString &fullParameterName, cParameterContainer *param,
		const cFractalContainer *fractal) override;
	void RemoveAnimatedParameter(const QString &fullParameterName) override;
	void setAudioParameterPrefix() override;

private:
	int framesPerKeyframe;
	QList<cMorph *> morph;
};

extern cKeyframes *gKeyframes;

#endif /* MANDELBULBER2_SRC_KEYFRAMES_HPP_ */
