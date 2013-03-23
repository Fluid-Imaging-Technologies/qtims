/*
  Copyright (c) 2013 Fluid Imaging Technologies
*/

#include "imgview.h"
#include <qboxlayout.h>

ImgView::ImgView(QWidget *parent, QSettings *settings, QString windowName)
	: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	m_settings = settings;
	m_windowName = windowName;
	m_imgValid = false;
	layoutWindow();
	restoreWindowState();
	setWindowTitle(m_windowName);
}

QString ImgView::windowName()
{
	return m_windowName;
}

void ImgView::setWindowName(QString windowName)
{
	m_windowName = windowName;
}

void ImgView::setImage(cv::Mat img)
{
	if (img.data) {
		if (CV_8UC3 == img.type()) {
			QImage tmp((const uchar*) img.data, img.cols, img.rows, QImage::Format_RGB888);
			m_qImg = tmp.rgbSwapped();
		}
		else {
			m_qImg = QImage((const uchar*) img.data, img.cols, img.rows, QImage::Format_Indexed8);

			for (int i = 0; i < 256; i++)
				m_qImg.setColor(i, qRgb(i, i, i));		
		}

		m_imgValid = true;

		if (minimumWidth() != (img.cols / 4))
			setMinimumWidth(img.cols/ 4);

		if (minimumHeight() != (img.rows / 4))
			setMinimumHeight(img.rows / 4);
	}
	else {
		m_imgValid = false;
	}

	displayImage();
}

void ImgView::closeEvent(QCloseEvent *)
{
	saveWindowState();
	emit closed(m_windowName);
}

void ImgView::resizeEvent(QResizeEvent *)
{
	displayImage();
}

void ImgView::displayImage()
{
	if (m_imgValid)
		m_imgLabel->setPixmap(QPixmap::fromImage(m_qImg.scaled(m_imgLabel->size(), Qt::KeepAspectRatio)));
	else
		m_imgLabel->setText(m_windowName);
}

void ImgView::layoutWindow()
{
	setMinimumSize(QSize(340, 260));

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(4, 4, 4, 4);

	m_imgLabel = new QLabel(m_windowName);
	m_imgLabel->setAlignment(Qt::AlignCenter);

	vLayout->addWidget(m_imgLabel);	
}

void ImgView::saveWindowState()
{
	if (!m_settings)
		return;

	m_settings->beginGroup(m_windowName);
	m_settings->setValue("Geometry", saveGeometry());
	m_settings->endGroup();
}

void ImgView::restoreWindowState()
{
	if (!m_settings)
		return;

	m_settings->beginGroup(m_windowName);
	restoreGeometry(m_settings->value("Geometry").toByteArray());
	m_settings->endGroup();
}
