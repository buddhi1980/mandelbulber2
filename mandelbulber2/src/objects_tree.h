/*
 * objects_treecpp.h
 *
 *  Created on: 24 lis 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OBJECTS_TREE_H_
#define MANDELBULBER2_SRC_OBJECTS_TREE_H_

#include <QString>
#include <QHash>
#include <memory>
#include <vector>
#include "object_node_type.h"
#include "algebra.hpp"

class cParameterContainer;
class cFractalContainer;

class cObjectsTree
{
public:
	struct sNodeData
	{
		QString name;
		int id;
		enumNodeType type;
		int parentId;
		int objectId;
		int level;
		int displayOrder;
		CVector3 position;
		CVector3 rotation;
		CVector3 repeat;
		double scale;
		int material;
		bool enabled = true;
		double detailLevelMultiplier = 1.0;
		// Common fractal parameters shared by all node types (including boolean groups)
		bool juliaMode = false;
		CVector3 juliaConstant;
		CVector3 fractalConstantMultiplier;
		double initialWAxis = 0.0;
		bool smoothDECombineEnable = false;
		double smoothDECombineDistance = 0.1;
		int formulaMaxiter = 250;
	};

	struct sNodeDataForRendering
	{
		int id;
		QString name;
		enumNodeType type;
		int parentId;
		int userObjectId;
		int internalObjectId;
		int primitiveIdx;
		int level;
		int hybridSequenceIndex;
		CVector3 position;
		CVector3 rotation;
		CVector3 repeat;
		double scale;
		double absScale;
		int material;
		CRotationMatrix rotationMatrix;
		CMatrix44 worldToLocalMatrix;
		bool enabled = true;
		double detailLevelMultiplier = 1.0;
		bool smoothDECombineEnable = false;
		double smoothDECombineDistance = 0.1;
		// Common fractal parameters shared by all node types (including hybrid groups)
		bool juliaMode = false;
		CVector3 juliaConstant;
		CVector3 fractalConstantMultiplier;
		double initialWAxis = 0.0;
		int formulaMaxiter = 250;
	};

	// Lightweight node description used for UI listings (e.g. mouse click function combo box).
	// Parsed only from "node_XXXX_definition" parameters, without reading other node params.
	struct sNodeListItem
	{
		int id = 0;																 // node ID
		QString name;															 // node display name
		enumNodeType type = enumNodeType::fractal; // node type
		int parentId = 0;													 // parent node ID (0 = attached to World)
		int objectId = -1;												 // associated object ID (fractal / primitive / group)
		int displayOrder = 0;											 // tree display order stored in definition
		int level = 0;														 // nesting depth (0 = root node)
	};

	typedef QHash<int, sNodeData> nodeData_t;

public:
	cObjectsTree();

	void CreateNodeDataFromParameters(std::shared_ptr<const cParameterContainer> params);
	nodeData_t &GetNodeDataMap() { return nodeDataMap; }
	std::vector<cObjectsTree::sNodeData> GetSortedNodeDataList() const;

	std::vector<cObjectsTree::sNodeDataForRendering> GetNodeDataListForRendering();

	static void WriteInternalNodeID(int userObjectID, int internalObjectID, int primitiveIdx,
		std::vector<cObjectsTree::sNodeDataForRendering> *nodes);

	static void DebugPrintNodes(const std::vector<cObjectsTree::sNodeDataForRendering> &nodes);

	// Returns the list of node IDs (int) for all nodes that have a "node_XXXX_definition" parameter
	// in the given parameter container.
	static QList<int> GetDefinedNodeIds(std::shared_ptr<const cParameterContainer> params);

	// Returns a flat list of all tree nodes parsed from "node_XXXX_definition" parameters.
	// The list is sorted in tree display order (displayOrder, then node ID as tiebreaker)
	// and each item has its nesting level computed from parent references.
	static QList<sNodeListItem> GetListOfNodes(std::shared_ptr<const cParameterContainer> params);

	// Builds the node's own local-to-parent transform matrix from its
	// position/rotation/scale parameters: p_parent = R * (s * p_local) + t
	static CMatrix44 NodeLocalToWorldMatrix(const sNodeData &nodeData);

	// Returns the accumulated local-to-world transform matrix of the parent of the node
	// with the given ID (identity matrix for root-level nodes). The node's own "position"
	// parameter is expressed in this coordinate system, so a world point must be converted
	// by the inverse of this matrix before writing it to node_XXXX_position.
	// Requires CreateNodeDataFromParameters() to be called first.
	CMatrix44 GetParentWorldMatrix(int nodeId) const;

private:
	nodeData_t nodeDataMap;
};

#endif /* MANDELBULBER2_SRC_OBJECTS_TREE_H_ */
