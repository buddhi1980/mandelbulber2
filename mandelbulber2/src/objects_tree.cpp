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
				nodeData.juliaMode = params->Get<bool>("node" + suffix + "_julia_mode");
				nodeData.juliaConstant = params->Get<CVector3>("node" + suffix + "_julia_c");
				nodeData.fractalConstantMultiplier =
					params->Get<CVector3>("node" + suffix + "_fractal_constant_factor");
				nodeData.initialWAxis = params->Get<double>("node" + suffix + "_initial_waxis");
				nodeData.smoothDECombineEnable =
					params->Get<bool>("node" + suffix + "_smooth_de_combine_enable");
				nodeData.smoothDECombineDistance =
					params->Get<double>("node" + suffix + "_smooth_de_combine_distance");
				nodeData.formulaMaxiter = params->Get<int>("node" + suffix + "_formula_maxiter");

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

	// Accumulated world-space 4x4 transform matrix keyed by node ID.
	// worldMatrix represents: p_world = worldMatrix * p_local (with w=1).
	// For identity: upper 3x3 = identity, column 4 (m14/m24/m34) = zero, m44 = 1.
	struct AccumulatedTransform
	{
		CMatrix44 matrix;
		int material = -1;
		AccumulatedTransform()
		{
			// Identity matrix: p_world = p_local (no transform for root/missing parent)
			matrix.m11 = 1.0;
			matrix.m12 = 0.0;
			matrix.m13 = 0.0;
			matrix.m14 = 0.0;
			matrix.m21 = 0.0;
			matrix.m22 = 1.0;
			matrix.m23 = 0.0;
			matrix.m24 = 0.0;
			matrix.m31 = 0.0;
			matrix.m32 = 0.0;
			matrix.m33 = 1.0;
			matrix.m34 = 0.0;
			matrix.m41 = 0.0;
			matrix.m42 = 0.0;
			matrix.m43 = 0.0;
			matrix.m44 = 1.0;
		}
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

		// Build the local-to-world 4x4 matrix for this node from its local transform parameters.
		// Forward transform: p_world = R * (s * p_local) + t
		// Matrix layout (row-major): upper 3x3 = R * scale, column 4 = translation, row 4 = [0,0,0,1]
		CMatrix44 localToWorld;
		CVector3 rotationXYZ = nodeData.rotation * (M_PI / 180.0);
		CRotationMatrix rotMat;
		rotMat.SetRotation2(rotationXYZ);
		const CMatrix33 &R = rotMat.GetMatrix();
		double s = 1.0 / nodeData.scale;
		localToWorld.m11 = R.m11 * s;
		localToWorld.m12 = R.m21 * s;
		localToWorld.m13 = R.m31 * s;
		localToWorld.m21 = R.m12 * s;
		localToWorld.m22 = R.m22 * s;
		localToWorld.m23 = R.m32 * s;
		localToWorld.m31 = R.m13 * s;
		localToWorld.m32 = R.m23 * s;
		localToWorld.m33 = R.m33 * s;
		localToWorld.m14 = nodeData.position.x;
		localToWorld.m24 = nodeData.position.y;
		localToWorld.m34 = nodeData.position.z;
		localToWorld.m41 = 0.0;
		localToWorld.m42 = 0.0;
		localToWorld.m43 = 0.0;
		localToWorld.m44 = 1.0;

		// Accumulate: worldMatrix = parentMatrix * localMatrix
		// This composes transforms correctly: rotation, scale, and translation all chain properly.
		CMatrix44 worldMatrix = parentTransform.matrix * localToWorld;

		// Extract position from world matrix translation column
		CVector3 worldPosition(worldMatrix.m14, worldMatrix.m24, worldMatrix.m34);

		// Extract scale from the norm of any row of the 3x3 rotation*scale block
		double worldScale = sqrt(worldMatrix.m11 * worldMatrix.m11 + worldMatrix.m12 * worldMatrix.m12
														 + worldMatrix.m13 * worldMatrix.m13);

		// Extract rotation from the 3x3 block of worldMatrix
		// Build the rotation-only matrix by dividing each row by worldScale
		CMatrix33 worldRotMatrix;
		if (worldScale > 1e-12)
		{
			double invWScale = 1.0 / worldScale;
			worldRotMatrix.m11 = worldMatrix.m11 * invWScale;
			worldRotMatrix.m12 = worldMatrix.m12 * invWScale;
			worldRotMatrix.m13 = worldMatrix.m13 * invWScale;
			worldRotMatrix.m21 = worldMatrix.m21 * invWScale;
			worldRotMatrix.m22 = worldMatrix.m22 * invWScale;
			worldRotMatrix.m23 = worldMatrix.m23 * invWScale;
			worldRotMatrix.m31 = worldMatrix.m31 * invWScale;
			worldRotMatrix.m32 = worldMatrix.m32 * invWScale;
			worldRotMatrix.m33 = worldMatrix.m33 * invWScale;
		}
		CRotationMatrix worldRot(worldRotMatrix);
		CVector3 worldRotation = worldRot.GetRotation2();

		// Material: a parent group with material != -1 overrides all descendants (outermost group
		// wins).  Only when no ancestor has an override does the node use its own material setting.
		int effectiveMaterial =
			(parentTransform.material != -1) ? parentTransform.material : nodeData.material;

		// Repeat: each leaf has its own repeat (no inheritance).
		// Repeat is applied in local space during distance calculation.
		CVector3 leafRepeat = nodeData.repeat;

		// Store accumulated transform for children to inherit
		AccumulatedTransform myTransform;
		myTransform.matrix = worldMatrix;
		myTransform.material = effectiveMaterial;
		accumulatedTransforms[nodeData.id] = myTransform;

		nodeDataForRendering.position = worldPosition;
		nodeDataForRendering.rotation = worldRotation;
		nodeDataForRendering.rotationMatrix.SetRotation2(worldRotation * (M_PI / 180.0));
		nodeDataForRendering.scale = worldScale;
		nodeDataForRendering.material = effectiveMaterial;
		nodeDataForRendering.repeat = leafRepeat;
		nodeDataForRendering.detailLevelMultiplier = nodeData.detailLevelMultiplier;
		nodeDataForRendering.juliaMode = nodeData.juliaMode;
		nodeDataForRendering.juliaConstant = nodeData.juliaConstant;
		nodeDataForRendering.fractalConstantMultiplier = nodeData.fractalConstantMultiplier;
		nodeDataForRendering.initialWAxis = nodeData.initialWAxis;
		nodeDataForRendering.smoothDECombineEnable = nodeData.smoothDECombineEnable;
		nodeDataForRendering.smoothDECombineDistance = nodeData.smoothDECombineDistance;
		nodeDataForRendering.formulaMaxiter = nodeData.formulaMaxiter;

		// Pre-calculate the world-to-local transform matrix.
		// Inverse of the forward transform: p_local = worldToLocalMatrix * p_world (w=1).
		// For affine transform with orthogonal rotation: inv(R*s) = R^T / s, inv(t) = -R^T * t.
		{
			nodeDataForRendering.worldToLocalMatrix = worldMatrix.InverseAffine();
			nodeDataForRendering.absScale = fabs(worldScale);
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
