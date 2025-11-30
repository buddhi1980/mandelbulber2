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

void cObjectsTree::CreateNodeDataFromParameters(
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	// Each "nodeXXXX" parameter is a QString with comma-separated values representing:
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
		if (paramName.startsWith("node"))
		{
			QString paramValue = params->Get<QString>(paramName);
			QStringList parts = paramValue.split(',');

			if (parts.size() == 5)
			{
				NodeData nodeData;
				nodeData.name = parts[0];
				nodeData.id = parts[1].toInt();
				nodeData.type = parts[2].toInt();
				nodeData.parentId = parts[3].toInt();
				nodeData.objectId = parts[4].toInt();

				nodeDataMap.insert(nodeData.id, nodeData);
			}
		}
	}
}

QList<cObjectsTree::NodeData> cObjectsTree::GetSortedNodeDataList() const
{
	// Build adjacency list (childrenMap) and in-degree map (inDegree)
	QHash<int, QList<int>> childrenMap; // Maps parent ID to list of child IDs
	QHash<int, int> inDegree;						// Maps node ID to its in-degree (number of parents)

	// Initialize in-degree for all nodes to 0
	for (const NodeData &node : nodeDataMap)
	{
		inDegree[node.id] = 0;
	}

	// Populate childrenMap and update in-degree for each child
	for (const NodeData &node : nodeDataMap)
	{
		// If node has a parent and the parent exists in the map
		if (node.parentId != 0 && nodeDataMap.contains(node.parentId))
		{
			childrenMap[node.parentId].append(node.id); // Add child to parent's list
			inDegree[node.id]++;												// Increment in-degree for child
		}
	}

	// Collect all nodes with in-degree 0 (root nodes)
	QList<int> zeroInDegree;
	for (auto it = inDegree.begin(); it != inDegree.end(); ++it)
	{
		if (it.value() == 0) zeroInDegree.append(it.key());
	}
	// Sort root nodes by ID for deterministic order
	std::sort(zeroInDegree.begin(), zeroInDegree.end(), std::greater<int>());

	QList<NodeData> sortedList; // Result list

	QStack<int> stack;

	// Push all root nodes onto the stack
	for (int id : zeroInDegree)
		stack.push(id);

	// Process nodes in FILO (stack) order
	while (!stack.isEmpty())
	{
		int id = stack.pop();
		sortedList.append(nodeDataMap[id]);

		QList<int> children = childrenMap.value(id);
		std::sort(children.begin(), children.end(), std::greater<int>());

		// Push children onto the stack
		for (int childId : children)
		{
			inDegree[childId]--;
			if (inDegree[childId] == 0) stack.push(childId);
		}
	}
	return sortedList; // Return sorted list of nodes
}
