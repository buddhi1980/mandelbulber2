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

		// checking if it is the last node of hybrid sequence
		bool endOfHybridNode = false;
		if (hybridNodeEntered)
		{
			// checking next node
			if (nodeIndex + 1 < objectsNodes.size())
			{
				const cObjectsTree::sNodeDataForRendering &nextNode = objectsNodes[nodeIndex + 1];
				// end hybrid if next node is not fractal or level is less or equal to hybrid node level
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

			sequence = CreateSequence(sequence, generalPar, formulaIndices, singleFractal);

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
			fractal::enumFractalFormula(generalPar->Get<int>("formula", objectId));
	}
}

cHybridFractalSequences::sSequence cHybridFractalSequences::CreateSequence(sSequence seq,
	std::shared_ptr<const cParameterContainer> generalPar, std::vector<int> formulaIndices,
	bool singleFractal)
{
	bool isHybrid = !singleFractal;

	int maxN = 250; // FIXME separate for each sequence

	seq.length = maxN * 5;
	seq.seqence.resize(seq.length);
	seq.fractData.resize(formulaIndices.size());

	int numberOfFormulas = formulaIndices.size();
	seq.numberOfFractalsInTheSequence = numberOfFormulas;

	int repeatFrom = generalPar->Get<int>("repeat_from");

	int fractalNoInSeqnece = 0;
	int counter = 0;

	double maxBailout = 0.0;
	bool useDefaultBailout = generalPar->Get<bool>("use_default_bailout");
	double commonBailout = generalPar->Get<double>("bailout");

	// collecting data for fractals within the sequence
	for (int i = 0; i < formulaIndices.size(); i++)
	{
		int objectId = formulaIndices[i];

		fractal::enumFractalFormula formula = fractalsMap[objectId].formula;
		int indexOnFractalList = GetIndexOnFractalList(formula);
		cAbstractFractal *fractalObject = newFractalList[indexOnFractalList];

		seq.fractData[i].fractalFormulaObject = fractalObject;
		seq.fractData[i].formulaIterations = generalPar->Get<int>("formula_iterations", objectId);
		seq.fractData[i].formulaWeight = generalPar->Get<double>("formula_weight", objectId);
		seq.fractData[i].formulaStartIteration =
			generalPar->Get<int>("formula_start_iteration", objectId);
		seq.fractData[i].formulaStopIteration =
			generalPar->Get<int>("formula_stop_iteration", objectId);
		seq.fractData[i].checkForBailout = generalPar->Get<bool>("check_for_bailout", objectId);

		if (singleFractal) seq.fractData[i].checkForBailout = true;

		// decide if use addition of C constant
		bool addc = false;
		if (fractalObject->getCpixelAddition() == fractal::cpixelAlreadyHas)
		{
			addc = false;
		}
		else
		{
			addc = !generalPar->Get<bool>("dont_add_c_constant", objectId);
			if (fractalObject->getCpixelAddition() == fractal::cpixelDisabledByDefault) addc = !addc;
		}
		seq.fractData[i].addCConstant = addc;

		// default bailout or global one
		if (useDefaultBailout)
		{
			if (isHybrid)
				maxBailout = qMax(maxBailout, fractalObject->getDefaultBailout());
			else
				seq.fractData[i].bailout = fractalObject->getDefaultBailout();
		}
		else
		{
			seq.fractData[i].bailout = commonBailout;
		}

		// Julia parameters - local or global
		if (singleFractal)
		{
			seq.juliaEnabled = generalPar->Get<bool>("julia_mode", i + 1);
			seq.juliaConstant = generalPar->Get<CVector3>("julia_c", i + 1);
			seq.constantMultiplier = generalPar->Get<CVector3>("fractal_constant_factor", i + 1);
			seq.initialWAxis = generalPar->Get<double>("initial_waxis", i + 1);
			seq.formulaMaxiter = generalPar->Get<double>("formula_maxiter", i + 1);
		}
		else
		{
			seq.juliaEnabled = generalPar->Get<bool>("julia_mode");
			seq.juliaConstant = generalPar->Get<CVector3>("julia_c");
			seq.constantMultiplier = generalPar->Get<CVector3>("fractal_constant_factor");
			seq.initialWAxis = generalPar->Get<double>("initial_waxis");
			seq.formulaMaxiter = generalPar->Get<double>("N");
		}

		seq.useAdditionalBailoutCond = false;
		if (singleFractal)
		{
			if (fractalObject->getDeFunctionType() == fractal::pseudoKleinianDEFunction
					|| fractalObject->getDeFunctionType() == fractal::josKleinianDEFunction)
			{
				seq.useAdditionalBailoutCond = true;
			}
		}
	}

	// common bailout for all hybrid components
	if (isHybrid && useDefaultBailout)
	{
		for (int i = 0; i < formulaIndices.size(); i++)
		{
			seq.fractData[i].bailout = maxBailout;
		}
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

		// seq.seqence[i] = fractalNoInSeqnece;
		seq.seqence[i] =
			objectId - 1; // FIXME: temporaru solution for CalculateFractal to use NineFrcatas data

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
