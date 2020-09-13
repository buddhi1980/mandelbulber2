/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_SYSTEM_DIRECTORIES_HPP_
#define MANDELBULBER2_SRC_SYSTEM_DIRECTORIES_HPP_

#include <QString>

struct sSystemDirectories
{
	QString dataDirectoryPublic;
	QString dataDirectoryHidden;

	void SetDataDirectoryHidden(QString target) { dataDirectoryHidden = std::move(target); }
	void SetDataDirectoryPublic(QString target) { dataDirectoryPublic = std::move(target); }
	QString GetDataDirectoryPublic() const { return dataDirectoryPublic; }
	QString GetDataDirectoryHidden() const { return dataDirectoryHidden; }
	QString GetDataDirectoryUsed() const;

	QString GetSettingsFolder() const { return GetDataDirectoryUsed() + "settings"; }
	QString GetImagesFolder() const { return GetDataDirectoryUsed() + "images"; }
	QString GetSlicesFolder() const { return GetDataDirectoryUsed() + "slices"; }
	QString GetMaterialsFolder() const { return GetDataDirectoryUsed() + "materials"; }
	QString GetAnimationFolder() const { return GetDataDirectoryUsed() + "animation"; }
	QString GetGradientsFolder() const { return GetDataDirectoryUsed() + "gradients"; }

	QString GetQueueFolder() const { return dataDirectoryHidden + "queue"; }
	QString GetToolbarFolder() const { return dataDirectoryHidden + "toolbar"; }
	QString GetHttpCacheFolder() const { return dataDirectoryHidden + "httpCache"; }
	QString GetCustomWindowStateFolder() const { return dataDirectoryHidden + "customWindowState"; }
	QString GetQueueFractlistFile() const { return dataDirectoryHidden + "queue.fractlist"; }
	QString GetThumbnailsFolder() const { return dataDirectoryHidden + "thumbnails"; }
	QString GetAutosaveFile() const { return dataDirectoryHidden + ".autosave.fract"; }
	QString GetRecentFilesListFile() const { return dataDirectoryHidden + "files.recent"; }
	QString GetResolutionPresetsFile() const { return dataDirectoryHidden + "resolutionPresets.ini"; }
	QString GetNetrenderFolder() const { return dataDirectoryHidden + "netrender"; }
	QString GetOpenCLTempFolder() const { return dataDirectoryHidden + "openclTemp"; }
	QString GetOpenCLCustomFormulasFolder() const { return dataDirectoryHidden + "customFormulas"; }
	QString GetUndoFolder() const { return dataDirectoryHidden + "undo"; }

	QString homeDir;
	QString sharedDir;
	QString docDir;
};

extern sSystemDirectories systemDirectories;

#endif /* MANDELBULBER2_SRC_SYSTEM_DIRECTORIES_HPP_ */
