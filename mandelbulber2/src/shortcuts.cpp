/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2024 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#include "shortcuts.h"

#include <QKeySequence>

#include "parameters.hpp"

enumShortcuts cShortcuts::getShortcut(cParameterContainer *settings, int key, int keymodifier)
{
	QKeySequence k = (key | keymodifier);

	if (k.matches(kSeq(settings, "key_move_forward"))) return enumShortcuts::moveForward;
	if (k.matches(kSeq(settings, "key_move_backward"))) return enumShortcuts::moveBackward;
	if (k.matches(kSeq(settings, "key_move_left"))) return enumShortcuts::moveLeft;
	if (k.matches(kSeq(settings, "key_move_right"))) return enumShortcuts::moveRight;
	if (k.matches(kSeq(settings, "key_move_up"))) return enumShortcuts::moveUp;
	if (k.matches(kSeq(settings, "key_move_down"))) return enumShortcuts::moveDown;
	if (k.matches(kSeq(settings, "key_rotate_up"))) return enumShortcuts::rotateUp;
	if (k.matches(kSeq(settings, "key_rotate_down"))) return enumShortcuts::rotateDown;
	if (k.matches(kSeq(settings, "key_rotate_left"))) return enumShortcuts::rotateLeft;
	if (k.matches(kSeq(settings, "key_rotate_right"))) return enumShortcuts::rotateRight;
	if (k.matches(kSeq(settings, "key_roll_left"))) return enumShortcuts::rollLeft;
	if (k.matches(kSeq(settings, "key_roll_right"))) return enumShortcuts::rollRight;
	if (k.matches(kSeq(settings, "key_render"))) return enumShortcuts::render;
	if (k.matches(kSeq(settings, "key_stop"))) return enumShortcuts::stop;
	if (k.matches(kSeq(settings, "key_add_keyframe"))) return enumShortcuts::addKeyframe;
	if (k.matches(kSeq(settings, "key_modify_keyframe"))) return enumShortcuts::modifyKeyframe;

	return enumShortcuts::none;
}

QKeySequence cShortcuts::kSeq(const cParameterContainer *params, const QString keyName)
{
	return QKeySequence(params->Get<QString>(keyName), QKeySequence::PortableText);
}
