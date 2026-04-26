/*
 * objects_treecpp.cpp
 *
 *  Created on: 24 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree.h"

#include "src/parameters.hpp"
#include "src/fractal_container.hpp"
#include <QHash>
#include <QStack>

cObjectsTree::cObjectsTree()
{
	// TODO Auto-generated constructor stub
}

void cObjectsTree::CreateNodeDataFromParameters(std::shared_ptr<const cParameterContainer> params)
{
	// Each "node_XXXX_definition" parameter is a QString with comma-separated values representing:
	// name, id, type, parent_id, object_id
	// Example: "hybrid group 1,1,0,0,-1"
	// - name: Node display name (QString)
	// - id: Node unique integer ID
	// - type: Node type (int, from enumNodeType)
	// - parent_id: Parent node ID (int)
	// - object_id: Associated object ID (int, or -1 if not applicable)

	QStringList allParams = params->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (paramName.startsWith("node_") && paramName.endsWith("_definition"))
		{
			QString paramValue = params->Get<QString>(paramName);
			QStringList parts = paramValue.split(',');

			if (parts.size() == 5)
			{
				sNodeData nodeData;
				nodeData.name = parts[0];
				nodeData.id = parts[1].toInt();
				nodeData.type = enumNodeType(parts[2].toInt());
				nodeData.parentId = parts[3].toInt();
				nodeData.objectId = parts[4].toInt();
				nodeData.level = -1;

				// Extract the node ID suffix, e.g. "node_0001_definition" -> "_0001"
				QString suffix = paramName.mid(QString("node").length(),
					paramName.length() - QString("node").length() - QString("_definition").length());

				nodeData.position = params->Get<CVector3>("node" + suffix + "_position");
				nodeData.rotation = params->Get<CVector3>("node" + suffix + "_rotation");
				nodeData.repeat = params->Get<CVector3>("node" + suffix + "_repeat");
				nodeData.scale = params->Get<double>("node" + suffix + "_scale");

				nodeDataMap.insert(nodeData.id, nodeData);
			}
		}
	}
}

std::vector<cObjectsTree::sNodeData> cObjectsTree::GetSortedNodeDataList() const
{
	// Make a local copy of the node data map to update levels without modifying the original
	nodeData_t localNodeDataMap = nodeDataMap;

	// Build adjacency list: maps parent ID to a list of child IDs
	QHash<int, QList<int>> childrenMap;
	// Map to track the in-degree (number of incoming edges) for each node
	QHash<int, int> inDegree;

	// Initialize in-degree for all nodes to 0
	for (const sNodeData &node : nodeDataMap)
	{
		inDegree[node.id] = 0;
	}

	// Populate childrenMap and update in-degree for each child node
	for (const sNodeData &node : nodeDataMap)
	{
		// If the node has a parent and the parent exists in the map
		if (node.parentId != 0 && nodeDataMap.contains(node.parentId))
		{
			childrenMap[node.parentId].append(node.id); // Add child to parent's list
			inDegree[node.id]++;												// Increment in-degree for the child
		}
	}

	// Collect all nodes with in-degree 0 (root nodes)
	QList<int> zeroInDegree;
	for (auto it = inDegree.begin(); it != inDegree.end(); ++it)
	{
		if (it.value() == 0) zeroInDegree.append(it.key());
	}
	// Sort root nodes in descending order for deterministic processing with stack
	std::sort(zeroInDegree.begin(), zeroInDegree.end(), std::greater<int>());

	std::vector<sNodeData> sortedList; // Result list to store sorted nodes

	// Stack for FILO traversal; stores pairs of (nodeId, level)
	QStack<QPair<int, int>> stack;

	// Push all root nodes onto the stack with level 0
	for (int id : zeroInDegree)
		stack.push(qMakePair(id, 0));

	// Process nodes in FILO order using the stack
	while (!stack.isEmpty())
	{
		auto pair = stack.pop();
		int id = pair.first;
		int level = pair.second;
		// Set the level for the current node
		localNodeDataMap[id].level = level;
		// Add the node to the sorted result list
		sortedList.push_back(localNodeDataMap[id]);

		// Get the list of children for the current node
		QList<int> children = childrenMap.value(id);
		// Sort children in descending order for deterministic stack processing
		std::sort(children.begin(), children.end(), std::greater<int>());

		// For each child, decrement its in-degree and push to stack if it becomes 0
		for (int childId : children)
		{
			inDegree[childId]--;
			if (inDegree[childId] == 0) stack.push(qMakePair(childId, level + 1));
		}
	}
	// Return the sorted list of nodes with level information set
	return sortedList;
}

std::vector<cObjectsTree::sNodeDataForRendering> cObjectsTree::GetNodeDataListForRendering()
{
	std::vector<cObjectsTree::sNodeData> nodeList = GetSortedNodeDataList();

	int sequenceIndex = 0;

	// Map from node ID to accumulated (cumulative) position for position inheritance
	QHash<int, CVector3> accumulatedPositions;

	// Track which nodes are children of a hybrid node
	QHash<int, bool> isInsideHybrid;

	std::vector<sNodeDataForRendering> nodeDataList;
	for (const sNodeData &nodeData : nodeList)
	{
		sNodeDataForRendering nodeDataForRendering;
		nodeDataForRendering.id = nodeData.id;
		nodeDataForRendering.type = nodeData.type;
		nodeDataForRendering.parentId = nodeData.parentId;
		nodeDataForRendering.userObjectId = nodeData.objectId;
		nodeDataForRendering.level = nodeData.level;
		nodeDataForRendering.internalObjectId = -1;
		nodeDataForRendering.primitiveIdx = -1;

		// Calculate cumulative position: own position + parent's accumulated position
		CVector3 parentAccumulatedPosition(0, 0, 0);
		if (nodeData.parentId != 0 && accumulatedPositions.contains(nodeData.parentId))
		{
			parentAccumulatedPosition = accumulatedPositions[nodeData.parentId];
		}
		CVector3 cumulativePosition = nodeData.position + parentAccumulatedPosition;
		accumulatedPositions[nodeData.id] = cumulativePosition;
		nodeDataForRendering.position = cumulativePosition;

		// Determine if this node is inside a hybrid node
		bool parentIsHybrid = isInsideHybrid.value(nodeData.parentId, false);
		isInsideHybrid[nodeData.id] = parentIsHybrid || (nodeData.type == enumNodeType::hybrid);

		bool isSingleFractal = (nodeDataForRendering.type == enumNodeType::fractal) && !parentIsHybrid;

		if (nodeDataForRendering.type == enumNodeType::hybrid)
		{
			// hybrid node gets a sequence index (sequence will be created for it in CreateSequences)
			nodeDataForRendering.hybridSequenceIndex = sequenceIndex;
			sequenceIndex++;
		}
		else if (isSingleFractal)
		{
			// single fractal node also gets a sequence index
			nodeDataForRendering.hybridSequenceIndex = sequenceIndex;
			sequenceIndex++;
		}
		else
		{
			nodeDataForRendering.hybridSequenceIndex = -1;
		}

		nodeDataList.push_back(nodeDataForRendering);
	}
	return nodeDataList;
}

void cObjectsTree::WriteInternalNodeID(int userObjectID, int internalObjectID, int primitiveIdx,
	std::vector<cObjectsTree::sNodeDataForRendering> *nodes)
{
	for (sNodeDataForRendering &node : *nodes)
	{
		if (node.userObjectId == userObjectID)
		{
			node.internalObjectId = internalObjectID;
			node.primitiveIdx = primitiveIdx;
			return;
		}
	}
}
