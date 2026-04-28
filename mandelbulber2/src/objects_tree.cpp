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
				nodeData.material = params->Get<int>("node" + suffix + "_material");

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

	// Accumulated world-space transforms keyed by node ID
	struct AccumulatedTransform
	{
		CVector3 position{0, 0, 0};
		CVector3 rotation{0, 0, 0};
		CVector3 repeat{0, 0, 0};
		double scale = 1.0;
		int material = -1;
	};
	QHash<int, AccumulatedTransform> accumulatedTransforms;

	// Track whether a node is a hybrid or descends from one
	QHash<int, bool> isHybridOrInsideHybrid;

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

		// Get parent's accumulated transform (identity if no parent)
		AccumulatedTransform parentTransform;
		if (nodeData.parentId != 0 && accumulatedTransforms.contains(nodeData.parentId))
		{
			parentTransform = accumulatedTransforms[nodeData.parentId];
		}

		// Build parent rotation matrix to rotate child's local position into parent space
		CRotationMatrix parentRotMatrix;
		parentRotMatrix.SetRotation(parentTransform.rotation);

		// World position = parent_position + parent_rotation * (parent_scale * child_local_position)
		CVector3 scaledLocalPosition = nodeData.position * parentTransform.scale;
		CVector3 worldPosition =
			parentTransform.position + parentRotMatrix.RotateVector(scaledLocalPosition);

		// Rotations accumulate additively (Euler angles)
		CVector3 worldRotation = parentTransform.rotation + nodeData.rotation;

		// Scales multiply
		double worldScale = parentTransform.scale * nodeData.scale;

		// Material: use own material if valid, otherwise inherit from parent
		int worldMaterial = (nodeData.material >= 0) ? nodeData.material : parentTransform.material;

		// Repeat: inherit parent repeat if own repeat is zero vector, otherwise use own
		CVector3 worldRepeat =
			(nodeData.repeat.Length() > 0.0) ? nodeData.repeat : parentTransform.repeat;

		// Store accumulated transform for children to inherit
		AccumulatedTransform myTransform;
		myTransform.position = worldPosition;
		myTransform.rotation = worldRotation;
		myTransform.scale = worldScale;
		myTransform.material = worldMaterial;
		myTransform.repeat = worldRepeat;
		accumulatedTransforms[nodeData.id] = myTransform;

		nodeDataForRendering.position = worldPosition;
		nodeDataForRendering.rotation = worldRotation;
		nodeDataForRendering.scale = worldScale;
		nodeDataForRendering.material = worldMaterial;
		nodeDataForRendering.repeat = worldRepeat;

		// Determine if this node is a hybrid or descends from one
		bool parentIsHybridOrInside = isHybridOrInsideHybrid.value(nodeData.parentId, false);
		bool isHybrid = (nodeData.type == enumNodeType::hybrid);
		isHybridOrInsideHybrid[nodeData.id] = parentIsHybridOrInside || isHybrid;

		bool isSingleFractal = (nodeData.type == enumNodeType::fractal) && !parentIsHybridOrInside;

		if (isHybrid)
		{
			nodeDataForRendering.hybridSequenceIndex = sequenceIndex;
			sequenceIndex++;
		}
		else if (isSingleFractal)
		{
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
