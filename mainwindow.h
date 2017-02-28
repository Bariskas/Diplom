#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "player.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	//Display video m_frame in player UI
	void updatePlayerUI(QImage img);
	// Slot for the play push button.
	void on_pushButton_2_clicked();
private:
	Ui::MainWindowClass* ui;
	Player* myPlayer;
};

#endif // MAINWINDOW_H
