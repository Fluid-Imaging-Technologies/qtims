/*
  Copyright (c) 2013 Fluid Imaging Technologies
*/

#include "ims.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Ims w;
	w.show();
	return a.exec();
}
