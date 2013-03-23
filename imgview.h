/*
  Copyright (c) 2013 Fluid Imaging Technologies
*/

#ifndef IMGVIEW_H
#define IMGVIEW_H

#include <qdialog.h>
#include <qlabel.h>
#include <qsettings.h>

#include <opencv2/core/core.hpp>

class ImgView : public QDialog
{
	Q_OBJECT

public:
	ImgView(QWidget *parent, QSettings *settings, QString windowName);

	QString windowName();
	void setWindowName(QString windowName);

	void setImage(cv::Mat img);

signals:
	void closed(QString windowName);

protected:
	void closeEvent(QCloseEvent *);
	void resizeEvent(QResizeEvent *);

private:
	void saveWindowState();
	void restoreWindowState();
	void layoutWindow();
	void displayImage();

	QSettings *m_settings;
	QString m_windowName;
	QLabel *m_imgLabel;
	QImage m_qImg;
	bool m_imgValid;
};

#endif // IMGVIEW_H
