/*
  Copyright (c) 2013 Fluid Imaging Technologies
*/

#ifndef IMS_H
#define IMS_H

#include <QtGui/QMainWindow>
#include <qsettings.h>
#include <qlabel.h>
#include <qmap.h>

#include <opencv2/core/core.hpp>

#include "ui_ims.h"
#include "imgview.h"
#include "imagestore.h"

class Ims : public QMainWindow
{
	Q_OBJECT

public:
	Ims(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Ims();

public slots:
	void onLoadBackground();
	void onLoadRaw();

	void onViewClosed(QString windowName);

protected:
	void closeEvent(QCloseEvent *);

private:
	void saveWindowState();
	void restoreWindowState();
	void initStatusBar();
	void onLoad(QString name);
	void processImage();
	bool createProcImages();
	bool haveUserImages();
	bool createDiffImage();
	bool getProcParams();

	Ui::ImsClass ui;
	QSettings *m_settings;
	QMap<QString, ImageStore *> m_store;

	double m_binThreshold;
};

#endif // IMS_H
