/*
 * material_list_view.h
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_MANAGER_VIEW_H_
#define MANDELBULBER2_QT_MATERIAL_MANAGER_VIEW_H_

#include <qgroupbox.h>
#include "../src/material_item_view.h"
#include "../src/material_item_model.h"

class cMaterialManagerView: public QGroupBox
{
	Q_OBJECT;
public:
	cMaterialManagerView(QWidget *parent);
	~cMaterialManagerView();
	void SetModel(cMaterialItemModel *model);

private:
	cMaterialItemView *itemView;
};

#endif /* MANDELBULBER2_QT_MATERIAL_MANAGER_VIEW_H_ */
