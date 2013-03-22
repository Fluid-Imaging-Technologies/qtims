/*
  Copyright (c) 2013 Fluid Imaging Technologies
*/

#include "ims.h"
#include <qdir.h>
#include <qfiledialog.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

Ims::Ims(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	m_settings = new QSettings(QDir::currentPath() + "/" + "qtims.ini", QSettings::IniFormat);

	// preload images that want updates in the statusBar
	m_store.insert("Background", new ImageStore("Background"));
	m_store.insert("Raw", new ImageStore("Raw"));

	initStatusBar();

	connect(ui.actionExit, SIGNAL(triggered()), SLOT(close()));
	connect(ui.actionLoadBackground, SIGNAL(triggered()), SLOT(onLoadBackground()));
	connect(ui.actionLoadImage, SIGNAL(triggered()), SLOT(onLoadRaw()));

	restoreWindowState();
}

Ims::~Ims()
{
}

void Ims::closeEvent(QCloseEvent *)
{
	saveWindowState();
}

void Ims::onViewClosed(QString windowName)
{
	if (!m_store.contains(windowName))
		return;

	ImageStore *is = m_store.value(windowName);

	if (!is->m_view)
		return;

	disconnect(is->m_view, SIGNAL(closed(QString)), this, SLOT(onViewClosed(QString)));
	delete is->m_view;
	is->m_view = NULL;
	is->m_img.release();

	if (is->m_status)
		is->m_status->setText(is->m_name + ": <none>");		
}

void Ims::onLoadBackground()
{
	onLoad("Background");	
}

void Ims::onLoadRaw()
{
	onLoad("Raw");
}

void Ims::onLoad(QString name)
{
	QString dir;
	
	ImageStore *is = m_store.value(name);

	QString f = QFileDialog::getOpenFileName(this, is->m_name, is->m_dir, "Images (*.tif)");

	if (f.length() == 0)
		return;

	QFileInfo info(f);
	is->m_dir = info.absolutePath();

	cv::Mat img = cv::imread(f.toStdString(), -1);
	if (img.data == NULL)
		return;

	if (CV_8UC3 != img.type() && CV_8UC1 != img.type())
		return;

	is->m_img = img;

	if (!is->m_view) {
		is->m_view = new ImgView(this, m_settings, is->m_name);
		is->m_view->show();
		connect(is->m_view, SIGNAL(closed(QString)), this, SLOT(onViewClosed(QString)));
	}

	is->m_view->setImage(is->m_img);

	is->m_filename = info.fileName();

	if (is->m_status)
		is->m_status->setText(is->m_name + ": " + is->m_filename);

	processImage();	
}

void Ims::processImage()
{
	if (!createProcImages())
		return;

	if (!getProcParams())
		return;

	ImageStore *raw = m_store.value("Raw");
	ImageStore *background = m_store.value("Background");
	ImageStore *diff = m_store.value("Diff");
	ImageStore *binary = m_store.value("Binary");

	cv::absdiff(background->m_img, raw->m_img, diff->m_img);
	diff->m_view->setImage(diff->m_img);

	cv::threshold(diff->m_img, binary->m_img, m_binThreshold, 255.0, CV_THRESH_BINARY);
	binary->m_view->setImage(binary->m_img);
}

bool Ims::createProcImages()
{
	if (!haveUserImages())
		return false;

	ImageStore *raw = m_store.value("Raw");

	ImageStore *diff = m_store.value("Diff", NULL);
	if (!diff) {
		diff = new ImageStore("Diff");
		diff->m_img.create(raw->m_img.rows, raw->m_img.cols, raw->m_img.type());
		diff->m_view = new ImgView(this, m_settings, diff->m_name);
		diff->m_view->show();
		connect(diff->m_view, SIGNAL(closed(QString)), this, SLOT(onViewClosed(QString)));
		m_store.insert("Diff", diff);
	}

	ImageStore *binary = m_store.value("Binary", NULL);
	if (!binary) {
		binary = new ImageStore("Binary");
		binary->m_img.create(raw->m_img.rows, raw->m_img.cols, raw->m_img.type());
		binary->m_view = new ImgView(this, m_settings, binary->m_name);
		binary->m_view->show();
		connect(binary->m_view, SIGNAL(closed(QString)), this, SLOT(onViewClosed(QString)));
		m_store.insert("Binary", binary);
	}

	return true;
}

bool Ims::haveUserImages()
{
	ImageStore *is = m_store.value("Raw", NULL);
	if (!is)
		return false;

	if (is->m_img.empty())
		return false;

	is = m_store.value("Background", NULL);
	if (!is)
		return false;

	if (is->m_img.empty())
		return false;

	return true;	
}

bool Ims::getProcParams()
{
	m_binThreshold = ui.binThreshEdit->text().toDouble();

	if (m_binThreshold < 0.0 || m_binThreshold > 255.0) {
		m_binThreshold = 25.0;
		ui.binThreshEdit->setText(QString::number(m_binThreshold, 'f', 1));
	}

	return true;
}

void Ims::initStatusBar()
{
	QMapIterator<QString, ImageStore *> i(m_store);

	while (i.hasNext()) {
		i.next();
		ImageStore *is = i.value();
		is->m_status = new QLabel(is->m_name + ": <none>");
		is->m_status->setFrameShape(QFrame::Panel);
		is->m_status->setFrameShadow(QFrame::Sunken);
		ui.statusBar->addWidget(is->m_status);
	}
}

void Ims::saveWindowState()
{
	if (!m_settings)
		return;

	m_settings->beginGroup("Window");
	m_settings->setValue("Geometry", saveGeometry());
	m_settings->setValue("State", saveState());
	m_settings->endGroup();

	QMapIterator<QString, ImageStore *> i(m_store);

	while (i.hasNext()) {
		i.next();
		ImageStore *is = i.value();

		if (is->m_dir.length() > 0) {		
			m_settings->beginGroup(is->m_name);
			m_settings->setValue("LastDir", is->m_dir);
			m_settings->endGroup();
		}
	}
}

void Ims::restoreWindowState()
{
	if (!m_settings)
		return;

	m_settings->beginGroup("Window");
	restoreGeometry(m_settings->value("Geometry").toByteArray());
	restoreState(m_settings->value("State").toByteArray());
	m_settings->endGroup();

	QMapIterator<QString, ImageStore *> i(m_store);

	while (i.hasNext()) {
		i.next();
		ImageStore *is = i.value();
		m_settings->beginGroup(is->m_name);
		is->m_dir = m_settings->value("LastDir", QDir::currentPath()).toString();
		m_settings->endGroup();
	}

	m_binThreshold = 18.0;
	ui.binThreshEdit->setText(QString::number(m_binThreshold, 'f', 1));
}
