/*
 * rendering_configuration.hpp
 *
 *  Created on: 13 wrz 2015
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_RENDERING_CONFIGURATION_HPP_
#define MANDELBULBER2_SRC_RENDERING_CONFIGURATION_HPP_

class cRenderingConfiguration
{
public:
	cRenderingConfiguration();

	void DisableRefresh() {enableImageRefresh = false;}
	void EnableConsoleOutput() {enableConsoleOutput = true;}
	void DisableProgressiveRender() {enableProgressiveRender = false;}
	void EnableNetRender() {enableNetRender = true;}
	void DisableMultiThread() {enableMultiThread = false;}
	void EnableIgnoreErros() {enableIgnoreErrors = true;}
	void SetMaxRenderTime(double _maxRenderTime) {maxRenderTime = _maxRenderTime;}

	bool UseNetRender() const;
	bool UseImageRefresh() const;
	bool UseProgressive() const;
	bool UseRefreshRenderedList() const;
	bool UseConsoleOutput() const;
	bool UseRenderTimeEffects() const;
	bool UseIgnoreErrors() const;
	int GetNumberOfThreads() const;
	double GetMaxRenderTime()  const {return maxRenderTime;}
	int GetRefreshRate() const;

private:
	bool enableImageRefresh;
	bool enableConsoleOutput;
	bool enableProgressiveRender;
	bool enableNetRender;
	bool enableMultiThread;
	bool enableIgnoreErrors;
	double maxRenderTime;
	int refreshRate;
};

#endif /* MANDELBULBER2_SRC_RENDERING_CONFIGURATION_HPP_ */
