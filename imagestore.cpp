/*
  Copyright (c) 2013 Fluid Imaging Technologies
*/

#include "imagestore.h"

ImageStore::ImageStore(QString name)
{
	m_name = name;
	m_status = NULL;
	m_view = NULL;
}