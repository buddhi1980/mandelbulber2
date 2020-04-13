/*
 * system_directories.hpp
 *
 *  Created on: 13 kwi 2020
 *      Author: krzysztof
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

	QString homeDir;
	QString sharedDir;
	QString docDir;
};

extern sSystemDirectories systemDirectories;

#endif /* MANDELBULBER2_SRC_SYSTEM_DIRECTORIES_HPP_ */
