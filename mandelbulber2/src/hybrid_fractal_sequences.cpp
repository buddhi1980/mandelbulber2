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

void cHybridFractalSequences::CollectSequenceData(
	const std::shared_ptr<const cParameterContainer> &generalPar,
	const std::vector<int> &formulaIndices, bool singleFractal, bool isHybrid, sSequence &seq)
{
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
		seq.fractData[i].fractalParameters = fractalsMap[objectId];

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
			seq.juliaEnabled = generalPar->Get<bool>("julia_mode", objectId);
			seq.juliaConstant = generalPar->Get<CVector3>("julia_c", objectId);
			seq.constantMultiplier = generalPar->Get<CVector3>("fractal_constant_factor", objectId);
			seq.initialWAxis = generalPar->Get<double>("initial_waxis", objectId);
			seq.formulaMaxiter = generalPar->Get<double>("formula_maxiter", objectId);
		}

		if (fractalObject->getDeFunctionType() == fractal::pseudoKleinianDEFunction
				|| fractalObject->getDeFunctionType() == fractal::josKleinianDEFunction)
		{
			seq.fractData[i].useAdditionalBailoutCond = true;
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

	if (isHybrid)
	{
		seq.juliaEnabled = generalPar->Get<bool>("julia_mode");
		seq.juliaConstant = generalPar->Get<CVector3>("julia_c");
		seq.constantMultiplier = generalPar->Get<CVector3>("fractal_constant_factor");
		seq.initialWAxis = generalPar->Get<double>("initial_waxis");
		seq.formulaMaxiter = generalPar->Get<double>("N");
	}
}

cHybridFractalSequences::sSequence cHybridFractalSequences::CreateSequence(sSequence seq,
	std::shared_ptr<const cParameterContainer> generalPar, std::vector<int> formulaIndices,
	bool singleFractal)
{
	bool isHybrid = !singleFractal;
	seq.isHybrid = isHybrid;

	int maxN = 250; // FIXME separate for each sequence

	seq.length = maxN * 5;
	seq.seqence.resize(seq.length);
	seq.fractData.resize(formulaIndices.size());

	int numberOfFormulas = formulaIndices.size();
	seq.numberOfFractalsInTheSequence = numberOfFormulas;

	int repeatFrom = generalPar->Get<int>("repeat_from");

	int fractalNoInSeqnece = 0;
	int counter = 0;

	CollectSequenceData(generalPar, formulaIndices, singleFractal, isHybrid, seq);

	// generating the sequence
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

	bool forceDeltaDE =
		fractal::enumDEMethod(generalPar->Get<int>("delta_DE_method")) == fractal::forceDeltaDEMethod;

	bool forceAnalyticDE =
		fractal::enumDEMethod(generalPar->Get<int>("delta_DE_method")) == fractal::forceAnalyticDE;

	fractal::enumDEFunctionType optimizedDEType = fractal::withoutDEFunction;

	if (isHybrid)
	{
		seq.DEType = fractal::analyticDEType;

		if (fractal::enumDEFunctionType(generalPar->Get<int>("delta_DE_function"))
				== fractal::preferredDEFunction)
		{
			// finding preferred delta DE function

			// table to check the which DE type is the most popular
			int DEFunctionCount[fractal::numberOfDEFunctions + 1];
			for (int i = 1; i <= fractal::numberOfDEFunctions; i++)
				DEFunctionCount[i] = 0;

			for (int f = 0; f < numberOfFormulas; f++)
			{
				fractal::enumFractalFormula formula = fractalsMap[formulaIndices[f]].formula;
				int index = GetIndexOnFractalList(formula);

				// looking for the best DE function for DeltaDE mode

				// count usage of DE functions
				fractal::enumDEFunctionType DEFunction = newFractalList[index]->getDeFunctionType();
				if (DEFunction != fractal::withoutDEFunction)
				{
					DEFunctionCount[DEFunction] += seq.fractData[f].formulaIterations;
				}

				// looking if it's possible to use analyticDEType
				if (!forceDeltaDE && newFractalList[index]->getInternalId() != fractal::none)
				{
					if (optimizedDEType == fractal::withoutDEFunction)
					{
						optimizedDEType = DEFunction;
					}

					if (!forceAnalyticDE && newFractalList[index]->getDeType() == fractal::deltaDEType)
					{
						seq.DEType = fractal::deltaDEType;
						forceDeltaDE = true;
					}
				}
			} // next f

			// checking if used dIFS formula
			if (DEFunctionCount[fractal::customDEFunction] > 0)
			{
				seq.DEFunctionType = fractal::customDEFunction;
			}
			else // use method which used in the highest iteration count
			{
				int maxCount = -1;
				for (int i = 1; i <= fractal::numberOfDEFunctions; i++)
				{
					if (DEFunctionCount[i] > maxCount)
					{
						maxCount = DEFunctionCount[i];
						seq.DEFunctionType = fractal::enumDEFunctionType(i);
					}
				}
			}
		} // endif preferredDEFunction
		else
		{
			seq.DEFunctionType = fractal::enumDEFunctionType(generalPar->Get<int>("delta_DE_function"));

			// if any fractal is delta DE type, then whole sequence is delta DE
			for (int f = 0; f < numberOfFormulas; f++)
			{
				fractal::enumFractalFormula formula = fractalsMap[formulaIndices[f]].formula;
				int index = GetIndexOnFractalList(formula);
				if (newFractalList[index]->getDeType() == fractal::deltaDEType)
				{
					seq.DEType = fractal::deltaDEType;
					break;
				}
			}
		}

		if (forceDeltaDE) seq.DEType = fractal::deltaDEType;
		if (forceAnalyticDE) seq.DEType = fractal::analyticDEType;
	}
	else // not hybrid
	{
		fractal::enumFractalFormula formula = fractalsMap[formulaIndices[0]].formula;
		int index = GetIndexOnFractalList(formula);
		seq.DEType = newFractalList[index]->getDeType();
		seq.DEFunctionType = newFractalList[index]->getDeFunctionType();
		seq.DEAnalyticFunction = newFractalList[index]->getDeAnalyticFunction();

		if (forceDeltaDE) seq.DEType = fractal::deltaDEType;
		if (forceAnalyticDE) seq.DEType = fractal::analyticDEType;

		if (fractal::enumDEFunctionType(generalPar->Get<int>("delta_DE_function"))
				!= fractal::preferredDEFunction)
		{
			seq.DEFunctionType = fractal::enumDEFunctionType(generalPar->Get<int>("delta_DE_function"));

			switch (seq.DEFunctionType)
			{
				case fractal::logarithmicDEFunction:
					seq.DEAnalyticFunction = fractal::analyticFunctionLogarithmic;
					break;
				case fractal::linearDEFunction:
					seq.DEAnalyticFunction = fractal::analyticFunctionLinear;
					break;
				case fractal::pseudoKleinianDEFunction:
					seq.DEAnalyticFunction = fractal::analyticFunctionPseudoKleinian;
					break;
				case fractal::josKleinianDEFunction:
					seq.DEAnalyticFunction = fractal::analyticFunctionJosKleinian;
					break;
				case fractal::customDEFunction:
					seq.DEAnalyticFunction = fractal::analyticFunctionCustomDE;
					break;
				case fractal::maxAxisDEFunction:
					seq.DEAnalyticFunction = fractal::analyticFunctionMaxAxis;
					break;
				default: seq.DEAnalyticFunction = fractal::analyticFunctionLinear; break;
			}
		}
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
