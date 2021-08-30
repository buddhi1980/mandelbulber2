/*
 * texture_cache.cpp
 *
 *  Created on: 30 sie 2021
 *      Author: krzysztof
 */

#include "texture_cache.h"
#include "texture.hpp"

std::shared_ptr<cTextureCache> gTextureCache;

cTextureCache::cTextureCache()
{

	cache.setMaxCost(4096 * 1024);
}

cTextureCache::~cTextureCache()
{
	// TODO Auto-generated destructor stub
}

void cTextureCache::setMaxSize(size_t _maxSizeKB)
{
	mutex.lock();

	cache.setMaxCost(_maxSizeKB);

	mutex.unlock();
}

void cTextureCache::AddToCache(cTexture *tex)
{
	mutex.lock();
	QString filename = tex->GetFileName();
	if (!cache.contains(filename))
	{
		cTexture *textureCopy = new cTexture(*tex);

		int cost = tex->GetMemorySize() / 1024;
		cache.insert(filename, textureCopy, cost);
	}
	mutex.unlock();
}

cTexture *cTextureCache::GetTexture(const QString &requestedFileName)
{
	mutex.lock();
	if (cache.contains(requestedFileName))
	{
		cTexture *outTexture = cache[requestedFileName];
		mutex.unlock();
		return outTexture;
	}
	else
	{
		mutex.unlock();
		return nullptr;
	}
}
