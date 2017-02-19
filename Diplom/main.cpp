#include "diplom.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Diplom w;
	w.show();
	return a.exec();
}
