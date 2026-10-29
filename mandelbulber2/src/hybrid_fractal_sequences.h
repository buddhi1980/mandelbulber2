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
		// Nested struct (as in OpenCL)
		sFractal fractalParameters; // data from fractal container for the fractal used in the sequence

		// Doubles (as in OpenCL)
		double formulaWeight;
		double bailout;

		// Ints (as in OpenCL)
		int formulaIterations;
		int formulaStartIteration;
		int formulaStopIteration;

		// Bools (as in OpenCL)
		bool addCConstant;
		bool checkForBailout;
		bool useAdditionalBailoutCond;

		// Non-OpenCL types (CPU-only)
		cAbstractFractal *fractalFormulaObject;
		int objectId = 0;
	};

	struct sSequence
	{
		// Vectors (as in OpenCL)
		CVector3 juliaConstant;
		CVector3 constantMultiplier;

		// Float (as in OpenCL)
		double initialWAxis = 0.0;

		// Ints (as in OpenCL)
		int length = 0;			 // number of iterations covered by valid (in-range) formulas
		int tableLength = 0; // size of the seqence[] table (formulaMaxiter * 5)
		int numberOfFractalsInTheSequence = 0;
		int internalObjectId = -1;

		// Bools + int (as in OpenCL)
		bool isHybrid = false;
		bool juliaEnabled = false;
		int formulaMaxiter = 0;

		// Ints (as in OpenCL)
		int sequenceArrayOffset = 0; // offset to sequence array (cl_int[]) in dynamic data
		int fractDataArrayOffset =
			0; // offset to fractData array (sHybridFractalDataCl[]) in dynamic data
		int formulaBaseIndex =
			0; // global index of first formula in consts->fractal[] for this sequence

		// Enums (last, as in OpenCL)
		fractal::enumDEFunctionType DEFunctionType = fractal::undefinedDEFunction;
		fractal::enumDEType DEType = fractal::undefinedDEType;
		fractal::enumDEAnalyticFunction DEAnalyticFunction = fractal::analyticFunctionUndefined;
		fractal::enumColoringFunction coloringFunction = fractal::coloringFunctionDefault;

		// Non-OpenCL types (kept for CPU use)
		std::vector<int> seqence;
		std::vector<sFractalData> fractData;

		// Returns the fractal index to be used at iteration i. The table covers the whole
		// possible loop range (formulaMaxiter * 5); the clamp is only a safety net and
		// mirrors the OpenCL behavior (repeat the last table entry, as in old cNineFractals).
		inline int GetSequence(const int i) const
		{
			if (tableLength <= 0) return 0;
			if (i < tableLength)
				return seqence[i];
			else
				return seqence[tableLength - 1];
		}
	};

	void CreateSequences(std::shared_ptr<const cParameterContainer> generalPar,
		std::shared_ptr<const cFractalContainer> par,
		const std::vector<cObjectsTree::sNodeDataForRendering> &_objectsNodes);
	sSequence *GetSequence(int seqIndex)
	{
		return (seqIndex >= 0 && seqIndex < static_cast<int>(sequences.size())) ? &sequences[seqIndex]
																																						: nullptr;
	}
	const sSequence *GetSequence(int seqIndex) const
	{
		return (seqIndex >= 0 && seqIndex < static_cast<int>(sequences.size())) ? &sequences[seqIndex]
																																						: nullptr;
	}
	int GetNumberOfSequences() const { return sequences.size(); };
	static int GetIndexOnFractalListStatic(fractal::enumFractalFormula formula);

private:
	void PrepareData(std::shared_ptr<const cParameterContainer> generalPa,
		std::shared_ptr<const cFractalContainer> parr);
	sSequence CreateSequence(sSequence seq, std::shared_ptr<const cParameterContainer> generalPar,
		std::vector<int> formulaIndices, bool singleFractal, int firstNodeIndex, int hybridNodeId = -1);
	int GetIndexOnFractalList(fractal::enumFractalFormula formula);
	void CollectSequenceData(const std::shared_ptr<const cParameterContainer> &generalPar,
		const std::vector<int> &formulaIndices, bool singleFractal, bool isHybrid, int firstNodeIndex,
		int hybridNodeId, sSequence &seq);
	void DebugOutput();

	cObjectsTree objectsTree;
	std::vector<cObjectsTree::sNodeDataForRendering> objectsNodes;

	std::vector<sSequence> sequences;
	QMap<int, sFractal> fractalsMap; //<objectId, fractal>
};

#endif /* MANDELBULBER2_SRC_HYBRID_FRACTAL_SEQUENCES_H_ */
