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

class cParameterContainer;
class cFractalContainer;

class cObjectsTree
{
public:
	struct NodeData
	{
		QString name;
		int id;
		int type;
		int parentId;
		int objectId;
		int level;
	};

	typedef QHash<int, NodeData> nodeData_t;

public:
	cObjectsTree();

	void CreateNodeDataFromParameters(
		std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams);
	nodeData_t &GetNodeDataMap() { return nodeDataMap; }

	QList<cObjectsTree::NodeData> GetSortedNodeDataList() const;

private:
	nodeData_t nodeDataMap;
};

#endif /* MANDELBULBER2_SRC_OBJECTS_TREE_H_ */
