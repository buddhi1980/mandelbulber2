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
	};

	struct sNodeDataForRendering
	{
		int id;
		enumNodeType type;
		int parentId;
		int userObjectId;
		int internalObjectId;
		int primitiveIdx;
		int level;
		int hybridSequenceIndex;
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

private:
	nodeData_t nodeDataMap;
};

#endif /* MANDELBULBER2_SRC_OBJECTS_TREE_H_ */
