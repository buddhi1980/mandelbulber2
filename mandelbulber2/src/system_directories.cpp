/*
 * system_directories.cpp
 *
 *  Created on: 13 kwi 2020
 *      Author: krzysztof
 */

#include "system_directories.hpp"
#include "system_data.hpp"

sSystemDirectories systemDirectories;

QString sSystemDirectories::GetDataDirectoryUsed() const
{
	return systemData.IsUpgraded() ? dataDirectoryPublic : dataDirectoryHidden;
}
