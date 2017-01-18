/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * This class collects all audio tracks (cAudioTrack objects)
 */

#ifndef MANDELBULBER2_SRC_AUDIO_TRACK_COLLECTION_H_
#define MANDELBULBER2_SRC_AUDIO_TRACK_COLLECTION_H_

#include <QtCore>

// forward declarations
class cAudioTrack;
class cParameterContainer;

class cAudioTrackCollection
{
public:
	cAudioTrackCollection();
	~cAudioTrackCollection();

	/* Warning! this is fake constructor to avoid copying audio data to cUndo buffers */
	cAudioTrackCollection(const cAudioTrackCollection &collection);
	/* Warning! this is fake operator to avoid copying audio data to cUndo buffers */
	cAudioTrackCollection &operator=(const cAudioTrackCollection &collection);

	void AddAudioTrack(const QString fullParameterName, cParameterContainer *params);
	void DeleteAudioTrack(const QString fullParameterName, cParameterContainer *params);
	void DeleteAllAudioTracks(cParameterContainer *params);
	cAudioTrack *GetAudioTrackPtr(const QString fullParameterName) const;
	void AddParameters(cParameterContainer *params, const QString parameterName);
	void RemoveParameters(cParameterContainer *params, const QString parameterName);
	QString FullParameterName(const QString &nameOfSoundParameter, const QString parameterName);
	void LoadAllAudioFiles(cParameterContainer *params);
	void RefreshAllAudioTracks(cParameterContainer *params);
	void SetPrefix(QString _prefix) { prefix = _prefix; }

private:
	QMap<QString, cAudioTrack *> audioTracks;
	QString prefix;
};

#endif /* MANDELBULBER2_SRC_AUDIO_TRACK_COLLECTION_H_ */
