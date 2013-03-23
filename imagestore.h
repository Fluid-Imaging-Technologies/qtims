/*
  Copyright (c) 2013 Fluid Imaging Technologies
*/

#ifndef IMAGESTORE_H
#define IMAGESTORE_H

#include "imgview.h"

class ImageStore
{
public:
	ImageStore(QString name);

	QString m_name;
	QString m_filename;
	QString m_dir;
	QLabel *m_status;
	ImgView *m_view;
	cv::Mat m_img;

private:
	ImageStore(const ImageStore&);
	ImageStore& operator=(const ImageStore&);
};

#endif // IMAGESTORE_H
