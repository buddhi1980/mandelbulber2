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
#include "algebra.hpp"
#include "formula/definition/all_fractal_list.hpp"
#include "fractal.h"

// forward declarations
class cParameterContainer;
class cFractalContainer;
class cAbstractFractal;
struct sFractal;

class cHybridFractalSequences
{
public:
	cHybridFractalSequences();

	struct sFractalData
	{
		double formulaWeight;
		int formulaIterations;
		int formulaStartIteration;
		int formulaStopIteration;
		bool addCConstant;
		bool checkForBailout;
		double bailout;
		bool useAdditionalBailoutCond;
		cAbstractFractal *fractalFormulaObject;
		sFractal fractalParameters; // data from fractal container for the fractal used in the sequence
	};

	struct sSequence
	{
		std::vector<int> seqence;						 // hybrud fractal sequence - indexes to fractData
		std::vector<sFractalData> fractData; // data for each fractal used in the sequence
		int length;													 // length of the seqence
		int numberOfFractalsInTheSequence;	 // number of different fractals used in the sequence

		fractal::enumDEFunctionType DEFunctionType;
		fractal::enumDEType DEType;
		fractal::enumDEAnalyticFunction DEAnalyticFunction;
		fractal::enumColoringFunction coloringFunction;

		bool isHybrid;
		bool juliaEnabled;
		CVector3 juliaConstant;
		CVector3 constantMultiplier;
		double initialWAxis;
		int formulaMaxiter;

		inline int GetSequence(const int i) const
		{
			if (i < length)
				return seqence[i];
			else
				return seqence[0];
		}
	};

	void CreateSequences(std::shared_ptr<const cParameterContainer> generalPar,
		std::shared_ptr<const cFractalContainer> par);
	sSequence *GetSequence(int seqIndex) { return &sequences[seqIndex]; };

private:
	void PrepareData(std::shared_ptr<const cParameterContainer> generalPa,
		std::shared_ptr<const cFractalContainer> parr);
	sSequence CreateSequence(sSequence seq, std::shared_ptr<const cParameterContainer> generalPar,
		std::vector<int> formulaIndices, bool singleFractal);
	int GetIndexOnFractalList(fractal::enumFractalFormula formula);
	void CollectSequenceData(const std::shared_ptr<const cParameterContainer> &generalPar,
		const std::vector<int> &formulaIndices, bool singleFractal, bool isHybrid, sSequence &seq);

	cObjectsTree objectsTree;
	std::vector<cObjectsTree::sNodeDataForRendering> objectsNodes;

	std::vector<sSequence> sequences;
	QMap<int, sFractal> fractalsMap; //<objectId, fractal>
};

#endif /* MANDELBULBER2_SRC_HYBRID_FRACTAL_SEQUENCES_H_ */
