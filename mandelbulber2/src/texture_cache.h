/*
 * texture_cache.h
 *
 *  Created on: 30 sie 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_TEXTURE_CACHE_H_
#define MANDELBULBER2_SRC_TEXTURE_CACHE_H_

#include <QCache>
#include <QMutex>
#include <QString>
#include <memory>

class cTexture;

class cTextureCache
{
public:
	cTextureCache();
	~cTextureCache();

	void setMaxSize(size_t _maxSize);
	void AddToCache(cTexture *tex);
	cTexture *GetTexture(const QString &requestedFileName);

	QCache<QString, cTexture> cache;
	QMutex mutex;
};

extern std::shared_ptr<cTextureCache> gTextureCache;

#endif /* MANDELBULBER2_SRC_TEXTURE_CACHE_H_ */
