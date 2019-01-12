/*
 * rendered_tile_data.hpp
 *
 *  Created on: 12 sty 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_RENDERED_TILE_DATA_HPP_
#define MANDELBULBER2_SRC_RENDERED_TILE_DATA_HPP_

struct sRenderedTileData
{
	sRenderedTileData(int _x, int _y, int _width, int _height, double _noiseLevel)
			: x(_x), y(_y), width(_width), height(_height), noiseLevel(_noiseLevel){};
	int x;
	int y;
	int width;
	int height;
	double noiseLevel;
};

#endif /* MANDELBULBER2_SRC_RENDERED_TILE_DATA_HPP_ */
