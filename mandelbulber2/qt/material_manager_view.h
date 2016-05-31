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
	void SetSelection(QModelIndex index);

private:
	Ui::cMaterialManagerView *ui;
	cMaterialItemView *itemView;
	cMaterialItemModel *model;

private slots:
	void slotAddMaterial();
	void slotDeleteMaterial();
	void slotEditMaterial();
	void slotLoadMaterial();
	void slotSaveMaterial();
	void slotItemSelected(const QModelIndex &index);

signals:
	void materialSelected(int matIndex);
	void materialEdited();
};

#endif /* MANDELBULBER2_QT_MATERIAL_MANAGER_VIEW_H_ */
