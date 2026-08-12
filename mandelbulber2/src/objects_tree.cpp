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
#include <QDebug>

cObjectsTree::cObjectsTree()
{
	// TODO Auto-generated constructor stub
}

QList<int> cObjectsTree::GetDefinedNodeIds(std::shared_ptr<const cParameterContainer> params)
{
	QList<int> nodeIds;
	QStringList allParams = params->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (paramName.startsWith("node_") && paramName.endsWith("_definition"))
		{
			// Extract the node ID suffix, e.g. "node_0001_definition" -> "0001"
			QString suffix = paramName.mid(5, paramName.length() - 5 - 11);
			nodeIds.append(suffix.toInt());
		}
	}
	return nodeIds;
}

void cObjectsTree::CreateNodeDataFromParameters(std::shared_ptr<const cParameterContainer> params)
{
	// Each "node_XXXX_definition" parameter is a QString with comma-separated values representing:
	// name, id, type, parent_id, object_id, displayOrder
	// Example: "hybrid group 1,1,0,0,-1,0"
	// - name: Node display name (QString)
	// - id: Node unique integer ID
	// - type: Node type (int, from enumNodeType)
	// - parent_id: Parent node ID (int)
	// - object_id: Associated object ID (int, or -1 if not applicable)
	// - displayOrder: Tree display order (int, 0 = default/legacy)

	QStringList allParams = params->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (paramName.startsWith("node_") && paramName.endsWith("_definition"))
		{
			QString paramValue = params->Get<QString>(paramName);
			QStringList parts = paramValue.split(',');

			if (parts.size() >= 5)
			{
				sNodeData nodeData;
				nodeData.name = parts[0];
				nodeData.id = parts[1].toInt();
				nodeData.type = enumNodeType(parts[2].toInt());
				nodeData.parentId = parts[3].toInt();
				nodeData.objectId = parts[4].toInt();
				nodeData.level = -1;
				nodeData.displayOrder = parts.size() >= 6 ? parts[5].toInt() : 0;

				// Extract the node ID suffix, e.g. "node_0001_definition" -> "_0001"
				QString suffix = paramName.mid(QString("node").length(),
					paramName.length() - QString("node").length() - QString("_definition").length());

				nodeData.position = params->Get<CVector3>("node" + suffix + "_position");
				nodeData.rotation = params->Get<CVector3>("node" + suffix + "_rotation");
				nodeData.repeat = params->Get<CVector3>("node" + suffix + "_repeat");
				nodeData.scale = params->Get<double>("node" + suffix + "_scale");
				nodeData.material = params->Get<int>("node" + suffix + "_material");
				nodeData.enabled = params->Get<bool>("node" + suffix + "_enabled");
				nodeData.detailLevelMultiplier =
					params->Get<double>("node" + suffix + "_detail_level_multiplier");

				// Common fractal parameters shared by all node types (including boolean groups)
				nodeData.julia_mode = params->Get<bool>("node" + suffix + "_julia_mode");
				nodeData.julia_c = params->Get<CVector3>("node" + suffix + "_julia_c");
				nodeData.fractal_constant_factor =
					params->Get<CVector3>("node" + suffix + "_fractal_constant_factor");
				nodeData.initial_waxis = params->Get<double>("node" + suffix + "_initial_waxis");
				nodeData.smooth_de_combine_enable =
					params->Get<bool>("node" + suffix + "_smooth_de_combine_enable");
				nodeData.smooth_de_combine_distance =
					params->Get<double>("node" + suffix + "_smooth_de_combine_distance");
				nodeData.formula_maxiter = params->Get<int>("node" + suffix + "_formula_maxiter");
				nodeData.formula_stop_iteration =
					params->Get<int>("node" + suffix + "_formula_stop_iteration");

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
	// Sort root nodes by displayOrder (ascending), then by nodeId as tiebreaker for determinism
	std::sort(zeroInDegree.begin(), zeroInDegree.end(),
		[this](int a, int b)
		{
			if (nodeDataMap[a].displayOrder != nodeDataMap[b].displayOrder)
				return nodeDataMap[a].displayOrder < nodeDataMap[b].displayOrder;
			return a < b;
		});

	std::vector<sNodeData> sortedList; // Result list to store sorted nodes

	// Stack for FILO traversal; stores pairs of (nodeId, level)
	QStack<QPair<int, int>> stack;

	// Push all root nodes onto the stack in reverse order so first one is popped first
	for (int i = zeroInDegree.size() - 1; i >= 0; --i)
		stack.push(qMakePair(zeroInDegree[i], 0));

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
		// Sort children by displayOrder (ascending), then by nodeId as tiebreaker for determinism
		std::sort(children.begin(), children.end(),
			[this](int a, int b)
			{
				if (nodeDataMap[a].displayOrder != nodeDataMap[b].displayOrder)
					return nodeDataMap[a].displayOrder < nodeDataMap[b].displayOrder;
				return a < b;
			});
		// Reverse for FILO stack processing
		std::reverse(children.begin(), children.end());

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

	// Build a parent-to-children adjacency map for enable propagation
	QHash<int, QList<int>> childrenByParent;
	for (const sNodeData &node : nodeDataMap)
	{
		if (node.parentId != 0)
		{
			childrenByParent[node.parentId].append(node.id);
		}
	}

	// Compute effective enabled state for each node (propagated from ancestors)
	QHash<int, bool> effectiveEnabled;
	QStack<QPair<int, bool>> enableStack; // (nodeId, parentEffectiveEnabled)
	// Start with root nodes (parentId == 0) as enabled
	for (const sNodeData &node : nodeDataMap)
	{
		if (node.parentId == 0)
		{
			enableStack.push(qMakePair(node.id, true));
		}
	}
	while (!enableStack.isEmpty())
	{
		auto pair = enableStack.pop();
		int nodeId = pair.first;
		bool parentEnabled = pair.second;
		auto nodeIt = nodeDataMap.find(nodeId);
		if (nodeIt == nodeDataMap.end()) continue;
		bool nodeEnabled = nodeIt->enabled;
		bool effective = parentEnabled && nodeEnabled;
		effectiveEnabled[nodeId] = effective;
		// Push children
		if (childrenByParent.contains(nodeId))
		{
			for (int childId : childrenByParent[nodeId])
			{
				enableStack.push(qMakePair(childId, effective));
			}
		}
	}

	std::vector<sNodeDataForRendering> nodeDataList;
	for (const sNodeData &nodeData : nodeList)
	{
		// Skip disabled nodes (and their children are already skipped by effectiveEnabled)
		if (!effectiveEnabled.value(nodeData.id, true))
		{
			continue;
		}

		sNodeDataForRendering nodeDataForRendering;
		nodeDataForRendering.id = nodeData.id;
		nodeDataForRendering.name = nodeData.name;
		nodeDataForRendering.type = nodeData.type;
		nodeDataForRendering.parentId = nodeData.parentId;
		nodeDataForRendering.userObjectId = nodeData.objectId;
		nodeDataForRendering.enabled = nodeData.enabled;
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
		parentRotMatrix.SetRotation2(parentTransform.rotation * (M_PI / 180.0));

		// World position = parent_position + parent_rotation * (parent_scale * child_local_position)
		CVector3 scaledLocalPosition = nodeData.position * parentTransform.scale;
		CVector3 worldPosition =
			parentTransform.position + parentRotMatrix.RotateVector(scaledLocalPosition);

		// Rotations accumulate additively (Euler angles)
		CVector3 worldRotation = parentTransform.rotation + nodeData.rotation;

		// Scales multiply
		double worldScale = parentTransform.scale * nodeData.scale;

		// Material: a parent group with material != -1 overrides all descendants (outermost group
		// wins).  Only when no ancestor has an override does the node use its own material setting.
		int effectiveMaterial =
			(parentTransform.material != -1) ? parentTransform.material : nodeData.material;

		// Repeat: each leaf has its own repeat (no inheritance).
		// Repeat is applied in local space during distance calculation.
		CVector3 leafRepeat = nodeData.repeat;

		// Store accumulated transform for children to inherit
		AccumulatedTransform myTransform;
		myTransform.position = worldPosition;
		myTransform.rotation = worldRotation;
		myTransform.scale = worldScale;
		myTransform.material = effectiveMaterial;
		myTransform.repeat = leafRepeat;
		accumulatedTransforms[nodeData.id] = myTransform;

		nodeDataForRendering.position = worldPosition;
		nodeDataForRendering.rotation = worldRotation;
		nodeDataForRendering.rotationMatrix.SetRotation2(worldRotation * (M_PI / 180.0));
		nodeDataForRendering.scale = worldScale;
		nodeDataForRendering.material = effectiveMaterial;
		nodeDataForRendering.repeat = leafRepeat;
		nodeDataForRendering.detailLevelMultiplier = nodeData.detailLevelMultiplier;
		nodeDataForRendering.julia_mode = nodeData.julia_mode;
		nodeDataForRendering.julia_c = nodeData.julia_c;
		nodeDataForRendering.fractal_constant_factor = nodeData.fractal_constant_factor;
		nodeDataForRendering.initial_waxis = nodeData.initial_waxis;
		nodeDataForRendering.smooth_de_combine_enable = nodeData.smooth_de_combine_enable;
		nodeDataForRendering.smooth_de_combine_distance = nodeData.smooth_de_combine_distance;
		nodeDataForRendering.formula_maxiter = nodeData.formula_maxiter;
		nodeDataForRendering.formula_stop_iteration = nodeData.formula_stop_iteration;

		// Pre-calculate the world-to-local transform matrix that combines
		// translation, rotation and scale into a single 4×4 homogeneous matrix.
		// Matches the legacy formula transform order: p_local = (1/S) * R * (p_world - t)
		// where S is the user-facing scale (inverted on load, like formulaScale in legacy code).
		{
			const double s = (worldScale != 0.0) ? (1.0 / worldScale) : 1.0;
			nodeDataForRendering.absScale = fabs(worldScale);

			const CMatrix33 &R = nodeDataForRendering.rotationMatrix.GetMatrix();

			// Upper-left 3×3 block = s * R (no transpose, matching legacy behavior)
			CMatrix44 &M = nodeDataForRendering.worldToLocalMatrix;
			M.m11 = s * R.m11;
			M.m12 = s * R.m12;
			M.m13 = s * R.m13;
			M.m21 = s * R.m21;
			M.m22 = s * R.m22;
			M.m23 = s * R.m23;
			M.m31 = s * R.m31;
			M.m32 = s * R.m32;
			M.m33 = s * R.m33;

			// Upper-right column = -s * R * position
			double rx = R.m11 * worldPosition.x + R.m12 * worldPosition.y + R.m13 * worldPosition.z;
			double ry = R.m21 * worldPosition.x + R.m22 * worldPosition.y + R.m23 * worldPosition.z;
			double rz = R.m31 * worldPosition.x + R.m32 * worldPosition.y + R.m33 * worldPosition.z;
			M.m14 = -s * rx;
			M.m24 = -s * ry;
			M.m34 = -s * rz;

			// Bottom row (not used by TransformPoint, set for completeness)
			M.m41 = 0.0;
			M.m42 = 0.0;
			M.m43 = 0.0;
			M.m44 = 1.0;
		}

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

void cObjectsTree::DebugPrintNodes(const std::vector<cObjectsTree::sNodeDataForRendering> &nodes)
{
	for (const sNodeDataForRendering &node : nodes)
	{
		QString part1 =
			QString("Node ID: %1, Name: %2, Type: %3, Parent ID: %4, User Object ID: %5, Level: %6")
				.arg(node.id)
				.arg(node.name)
				.arg(static_cast<int>(node.type))
				.arg(node.parentId)
				.arg(node.userObjectId)
				.arg(node.level);

		QString part2 = QString(" Position: (%1, %2, %3), Rotation: (%4, %5, %6)")
											.arg(QString::number(node.position.x, 'f', 4))
											.arg(QString::number(node.position.y, 'f', 4))
											.arg(QString::number(node.position.z, 'f', 4))
											.arg(QString::number(node.rotation.x, 'f', 4))
											.arg(QString::number(node.rotation.y, 'f', 4))
											.arg(QString::number(node.rotation.z, 'f', 4));

		QString part3 = QString(
			" Scale: %1, Material: %2, Repeat: (%3, %4, %5), Hybrid Seq Index: %6, Internal Object ID: "
			"%7, Primitive Idx: %8")
											.arg(QString::number(node.scale, 'f', 4))
											.arg(node.material)
											.arg(QString::number(node.repeat.x, 'f', 4))
											.arg(QString::number(node.repeat.y, 'f', 4))
											.arg(QString::number(node.repeat.z, 'f', 4))
											.arg(node.hybridSequenceIndex)
											.arg(node.internalObjectId)
											.arg(node.primitiveIdx);

		qDebug().noquote() << part1 + part2 + part3;
	}
}
