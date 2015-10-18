/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderingConfiguration - holds configuration for the render engine
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef MANDELBULBER2_SRC_RENDERING_CONFIGURATION_HPP_
#define MANDELBULBER2_SRC_RENDERING_CONFIGURATION_HPP_

class cRenderingConfiguration
{
public:
	cRenderingConfiguration();

	void DisableRefresh() {enableImageRefresh = false;}
	void DisableProgressiveRender() {enableProgressiveRender = false;}
	void EnableNetRender() {enableNetRender = true;}
	void DisableMultiThread() {enableMultiThread = false;}
	void EnableIgnoreErros() {enableIgnoreErrors = true;}
	void SetMaxRenderTime(double _maxRenderTime) {maxRenderTime = _maxRenderTime;}

	bool UseNetRender() const;
	bool UseImageRefresh() const;
	bool UseProgressive() const;
	bool UseRefreshRenderedList() const;
	bool UseRenderTimeEffects() const;
	bool UseIgnoreErrors() const;
	int GetNumberOfThreads() const;
	double GetMaxRenderTime()  const {return maxRenderTime;}
	int GetRefreshRate() const;

private:
	bool enableImageRefresh;
	bool enableProgressiveRender;
	bool enableNetRender;
	bool enableMultiThread;
	bool enableIgnoreErrors;
	double maxRenderTime;
	int refreshRate;
};

#endif /* MANDELBULBER2_SRC_RENDERING_CONFIGURATION_HPP_ */
