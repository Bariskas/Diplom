#ifndef DIPLOM_H
#define DIPLOM_H

#include <QtWidgets/QMainWindow>
#include "ui_diplom.h"

class Diplom : public QMainWindow
{
	Q_OBJECT

public:
	Diplom(QWidget *parent = 0);
	~Diplom();

private:
	Ui::DiplomClass ui;
};

#endif // DIPLOM_H
