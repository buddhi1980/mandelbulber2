/*
 * material_list_view.h
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_MANAGER_VIEW_H_
#define MANDELBULBER2_QT_MATERIAL_MANAGER_VIEW_H_

#include <QWidget>

#include "../src/material_item_view.h"
#include "../src/material_item_model.h"

namespace Ui {
	class cMaterialManagerView;
}

class cMaterialManagerView: public QWidget
{
	Q_OBJECT;
public:
	explicit cMaterialManagerView(QWidget *parent = NULL);
	~cMaterialManagerView();
	void SetModel(cMaterialItemModel *_model);

private:
	Ui::cMaterialManagerView *ui;
	cMaterialItemView *itemView;
	cMaterialItemModel *model;

private slots:
	void slotAddMaterial();
	void slotItemSelected(const QModelIndex &index);

private:
	int lastMaterialSelected;

signals:
	void materialSelected(int matIndex);
};

#endif /* MANDELBULBER2_QT_MATERIAL_MANAGER_VIEW_H_ */
