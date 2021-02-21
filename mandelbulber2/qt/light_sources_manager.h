/*
 * light_sources_manager.h
 *
 *  Created on: 3 sty 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_LIGHT_SOURCES_MANAGER_H_
#define MANDELBULBER2_QT_LIGHT_SOURCES_MANAGER_H_

#include <QWidget>

namespace Ui
{
class cLightSourcesManager;
}

class cLightSourcesManager : public QWidget
{
	Q_OBJECT

public:
	explicit cLightSourcesManager(QWidget *parent = nullptr);
	~cLightSourcesManager() override;
	void Regenerate();

private:
	void AddLight(bool init, int indexInParameters);

	QList<int> lightIndexOnTab;
	Ui::cLightSourcesManager *ui;

	QTimer *autoRefreshTimer;
	QString autoRefreshLastHash;

private slots:
	void slotButtonAddLight();
	void slotButtonDuplicateLight();
	void slotButtonDeleteLight();
	void slotPeriodicRefresh();
	void slorChangedWireframeVisibikity(int);
};

#endif /* MANDELBULBER2_QT_LIGHT_SOURCES_MANAGER_H_ */
