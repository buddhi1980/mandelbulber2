/*
 * hybrid_fractal_sequences.cpp
 *
 *  Created on: 24 gru 2025
 *      Author: krzysztof
 */

#include "hybrid_fractal_sequences.h"

#include <QSet>

#include "formula/definition/all_fractal_list.hpp"
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

	for (int nodeIndex = 0; nodeIndex < objectsNodes.size(); nodeIndex++)
	{
		const cObjectsTree::sNodeDataForRendering &node = objectsNodes[nodeIndex];

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

		bool endOfHybridNode = false;
		if (hybridNodeEntered)
		{
			if (nodeIndex + 1 < objectsNodes.size())
			{
				const cObjectsTree::sNodeDataForRendering &nextNode = objectsNodes[nodeIndex + 1];
				endOfHybridNode = nextNode.type != enumNodeType::fractal && nextNode.level <= levelOfHybrid;
			}
			else
			{
				// Last node: end hybrid if still inside
				endOfHybridNode = true;
			}
		}

		bool singleFractal = node.type == enumNodeType::fractal && !hybridNodeEntered;

		if (singleFractal)
		{
			formulaIndices.push_back(node.userObjectId);
		}

		if (endOfHybridNode || singleFractal)
		{
			// creating sequence for collected formula indices
			sSequence sequence;
			sequence.DEFunctionType = logarithmicDEFunction;					 // FIXME: later
			sequence.DEType = analyticDEType;													 // FIXME: later
			sequence.DEAnalyticFunction = analyticFunctionLogarithmic; // FIXME: later
			sequence.coloringFunction = coloringFunctionDefault;			 // FIXME: later
			CreateSequence(sequence, generalPar, formulaIndices);

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

cHybridFractalSequences::sSequence cHybridFractalSequences::CreateSequence(sSequence seq,
	std::shared_ptr<const cParameterContainer> generalPar, std::vector<int> formulaIndices)
{
	int maxN = 250; // FIXME separate for each sequence

	seq.length = maxN * 5;
	seq.seqence.resize(seq.length);
	seq.fractData.resize(formulaIndices.size());

	int numberOfFormulas = formulaIndices.size();

	int repeatFrom = generalPar->Get<int>("repeat_from");

	int fractalNoInSeqnece = 0;
	int counter = 0;

	// collecting data for fractals within the sequence
	for (int i = 0; i < formulaIndices.size(); i++)
	{
		int objectId = formulaIndices[i];

		seq.fractData[i].fractalFormulaObject =
			newFractalList[GetIndexOnFractalList(fractalsMap[objectId].formula)];

		seq.fractData[i].formulaIterations =
			generalPar->Get<int>("formula_iterations", objectId + 1); //+1 because objectId is 0-based
																																// in UI fractals starts from 1
		seq.fractData[i].formulaWeight = generalPar->Get<double>("formula_weight", objectId + 1);
		seq.fractData[i].formulaStartIteration =
			generalPar->Get<int>("formula_start_iteration", objectId + 1);
		seq.fractData[i].formulaStopIteration =
			generalPar->Get<int>("formula_stop_iteration", objectId + 1);
		seq.fractData[i].addCConstant = generalPar->Get<bool>("dont_add_c_constant", objectId + 1);
		seq.fractData[i].checkForBailout = generalPar->Get<bool>("check_for_bailout", objectId + 1);

		seq.fractData[i].bailout = seq.fractData[i].fractalFormulaObject->getDefaultBailout();
	}

	bool rapidEndOfSequence = false;
	int lastSequenceIndex = 0;

	for (int i = 0; i < seq.length; i++)
	{
		counter++;

		int objectId = formulaIndices[fractalNoInSeqnece];

		sFractalData &seqData = seq.fractData[fractalNoInSeqnece];

		int searchRepeatCount = 0;
		// skipping 'none' formulas and formulas out of iteration range
		while ((fractalsMap[objectId].formula == fractal::none || i < seqData.formulaStartIteration
						 || i > seqData.formulaStopIteration)
					 && searchRepeatCount < seq.fractData.size())
		{
			fractalNoInSeqnece++;

			// wrapping fractal number in sequence
			if (fractalNoInSeqnece >= seq.fractData.size()) fractalNoInSeqnece = 0;
			searchRepeatCount++;
		}

		// checking if all formulas are 'none' or out of range
		if (searchRepeatCount >= seq.fractData.size())
		{
			rapidEndOfSequence = true;
			break;
		}

		seq.seqence[i] = fractalNoInSeqnece;
		lastSequenceIndex = i;

		// advancing to next fractal in sequence if needed
		if (counter >= seqData.formulaIterations)
		{
			counter = 0;
			fractalNoInSeqnece++;
			if (fractalNoInSeqnece >= seq.fractData.size()) fractalNoInSeqnece = 0;
		}
	}

	if (rapidEndOfSequence)
	{
		seq.length = lastSequenceIndex + 1;
	}

	return seq;
}

int cHybridFractalSequences::GetIndexOnFractalList(fractal::enumFractalFormula formula)
{
	for (int i = 0; i < newFractalList.size(); i++)
	{
		if (newFractalList[i]->getInternalId() == formula)
		{
			return i;
		}
	}
	return 0;
}
