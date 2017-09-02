/*
 * opencl_engine_render_dof.h
 *
 *  Created on: 2 wrz 2017
 *      Author: krzysztof
 */

#include <QtCore>

class cOpenClEngineRenderDOFPhase1;
class cOpenClHardware;
struct sParamRender;
class cParameterContainer;
class cImage;

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_DOF_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_DOF_H_

class cOpenClEngineRenderDOF : public QObject
{
	Q_OBJECT
public:
	cOpenClEngineRenderDOF(cOpenClHardware *_hardware);
	~cOpenClEngineRenderDOF();

#ifdef USE_OPENCL
	bool RenderDOF(const sParamRender *paramRender, const cParameterContainer *params, cImage *image,
		bool *stopRequest);

	cOpenClEngineRenderDOFPhase1 *dofEnginePhase1;
#endif

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_DOF_H_ */
