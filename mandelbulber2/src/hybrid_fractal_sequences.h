/*
 * hybrid_fractal_sequences.h
 *
 *  Created on: 24 gru 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_HYBRID_FRACTAL_SEQUENCES_H_
#define MANDELBULBER2_SRC_HYBRID_FRACTAL_SEQUENCES_H_

#include <vector>
#include <memory>
#include <QMap>

#include "fractal_enums.h"
#include "objects_tree.h"

// forward declarations
class cParameterContainer;
class cFractalContainer;
struct sFractal;

class cHybridFractalSequences
{
public:
	cHybridFractalSequences();

	struct sSequence
	{
		std::vector<int> sequence;
		int length;

		fractal::enumDEFunctionType DEFunctionType;
		fractal::enumDEType DEType;
		fractal::enumDEAnalyticFunction DEAnalyticFunction;
		fractal::enumColoringFunction coloringFunction;
	};

	void CreateSequences(std::shared_ptr<const cParameterContainer> generalPar,
		std::shared_ptr<const cFractalContainer> par);

private:
	void PrepareData(std::shared_ptr<const cParameterContainer> generalPa,
		std::shared_ptr<const cFractalContainer> parr);
	sSequence CreateSequence(
		std::shared_ptr<const cParameterContainer> generalPar, std::vector<int> formulaIndices);

	cObjectsTree objectsTree;
	std::vector<cObjectsTree::sNodeDataForRendering> objectsNodes;

	std::vector<sSequence> sequences;
	QMap<int, sFractal> fractalsMap; //<objectId, fractal>
};

#endif /* MANDELBULBER2_SRC_HYBRID_FRACTAL_SEQUENCES_H_ */
