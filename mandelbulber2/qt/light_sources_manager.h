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

private:
	void AddLight();

	Ui::cLightSourcesManager *ui;
};

#endif /* MANDELBULBER2_QT_LIGHT_SOURCES_MANAGER_H_ */
