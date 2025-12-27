/*
 * hybrid_fractal_sequences.cpp
 *
 *  Created on: 24 gru 2025
 *      Author: krzysztof
 */

#include "hybrid_fractal_sequences.h"

#include <QSet>

#include "fractal.h"
#include "fractal_container.hpp"
#include "object_node_type.h"
#include "parameters.hpp"
#include "objects_tree.h"

using namespace fractal;

cHybridFractalSequences::cHybridFractalSequences()
{
	// TODO Auto-generated constructor stub
}

void cHybridFractalSequences::CreateSequences(std::shared_ptr<const cParameterContainer> generalPar,
	std::shared_ptr<const cFractalContainer> fractPar)
{
	PrepareData(generalPar, fractPar);

	// create sequences based on objects tree

	bool hybridNodeEntered = false;
	std::vector<int> formulaIndices;
	int levelOfHybrid = -1;

	for (const cObjectsTree::sNodeDataForRendering &node : objectsNodes)
	{
		if (node.type == enumNodeType::hybrid)
		{
			hybridNodeEntered = true;
			formulaIndices.clear();
			levelOfHybrid = node.level;
		}

		if (node.type == enumNodeType::fractal && hybridNodeEntered)
		{
			formulaIndices.push_back(node.userObjectId);
		}

		bool endOfHybridNode =
			node.type != enumNodeType::fractal && node.level <= levelOfHybrid && hybridNodeEntered;
		bool singleFractal = node.type == enumNodeType::fractal && !hybridNodeEntered;

		if (singleFractal)
		{
			formulaIndices.push_back(node.userObjectId);
		}

		if (endOfHybridNode || singleFractal)
		{
			// creating sequence for collected formula indices
			sSequence sequence = CreateSequence(generalPar, formulaIndices);
			sequence.DEFunctionType = undefinedDEFunction;					 // FIXME: later
			sequence.DEType = undefinedDEType;											 // FIXME: later
			sequence.DEAnalyticFunction = analyticFunctionUndefined; // FIXME: later
			sequence.coloringFunction = coloringFunctionUndefined;	 // FIXME: later
			sequences.push_back(sequence);

			hybridNodeEntered = false;
			formulaIndices.clear();
			levelOfHybrid = -1;
		}
	}
}

void cHybridFractalSequences::PrepareData(std::shared_ptr<const cParameterContainer> generalPar,
	std::shared_ptr<const cFractalContainer> fractPar)
{
	// preparing objects tree to get used fractal objects
	objectsTree.CreateNodeDataFromParameters(generalPar);
	objectsNodes = objectsTree.GetNodeDataListForRendering();

	// getting used fractal object indices
	QSet<int> usedFractalObjectIndices;
	for (const cObjectsTree::sNodeDataForRendering &node : objectsNodes)
	{
		if (node.type == enumNodeType::fractal)
		{
			usedFractalObjectIndices.insert(node.userObjectId);
		}
	}

	// creating fractals map
	for (int objectId : usedFractalObjectIndices)
	{
		sFractal fractal(fractPar->at(objectId - 1));
		fractalsMap.insert(objectId, fractal);

		// getting selected formula
		fractalsMap[objectId].formula =
			fractal::enumFractalFormula(generalPar->Get<int>("formula", objectId + 1));
	}
}

cHybridFractalSequences::sSequence cHybridFractalSequences::CreateSequence(
	std::shared_ptr<const cParameterContainer> generalPar, std::vector<int> formulaIndices)
{
	sSequence sequence;

	int maxN = 250; // FIXME separate for each sequence

	sequence.length = maxN * 5;
	sequence.sequence.resize(sequence.length);

	int numberOfFormulas = formulaIndices.size();

	int repeatFrom = generalPar->Get<int>("repeat_from");

	int fractalNo = 0;
	int counter = 0;

	std::vector<int> counts(numberOfFormulas);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		counts[i] = generalPar->Get<int>("formula_iterations", i + 1);
	}

	//	for (int i = 0; i < sequence.length; i++)
	//	{
	//		counter++;
	//
	//		int repeatCount = 0;
	//		while ((fractals[fractalNo]->formula == fractal::none || i <
	// formulaStartIteration[fractalNo]
	//						 || i > formulaStopIteration[fractalNo])
	//					 && repeatCount < NUMBER_OF_FRACTALS)
	//		{
	//			fractalNo++;
	//			if (fractalNo >= NUMBER_OF_FRACTALS) fractalNo = repeatFrom - 1;
	//			repeatCount++;
	//		}
	//		hybridSequence[i] = fractalNo;
	//		if (fractals[fractalNo]->formula != fractal::none && fractalNo > maxFractalIndex)
	//			maxFractalIndex = fractalNo;
	//
	//		if (counter >= counts[fractalNo])
	//		{
	//			counter = 0;
	//			fractalNo++;
	//			if (fractalNo >= NUMBER_OF_FRACTALS) fractalNo = repeatFrom - 1;
	//		}
	//	}
	//	else
	//	{
	//		hybridSequence[i] = 0;
	//	}

	return sequence;
}
