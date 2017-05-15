#include "mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindowClass)
{
	ui->setupUi(this);

	myPlayer = new Player();
	if (!myPlayer->loadVideo())
	{
		QMessageBox msgBox;
		msgBox.setText("Webcam not found");
		msgBox.exec();
	}
	QThread *playerThread = new QThread();
	myPlayer->moveToThread(playerThread);

	QObject::connect(myPlayer, SIGNAL(sendFrame(QImage)), this, SLOT(updatePlayerUI(QImage)));
	QObject::connect(playerThread, SIGNAL(started()), myPlayer, SLOT(processFrame()));
	QObject::connect(this, SIGNAL(togglePlayer()), myPlayer, SLOT(toggle()), Qt::DirectConnection);

	playerThread->start();
}

MainWindow::~MainWindow()
{
	delete myPlayer;
	delete ui;
}

void MainWindow::updatePlayerUI(QImage img)
{
	if (!img.isNull())
	{
		ui->label->setAlignment(Qt::AlignCenter);
		ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(),
			Qt::KeepAspectRatio, Qt::FastTransformation));
	}
}

void MainWindow::on_pushButton_2_clicked()
{
	emit togglePlayer();
	qDebug() << QString("on_pushButton_2_clicked");
	if (myPlayer->isRunning())
	{
		qDebug() << QString("Stop");
		ui->pushButton_2->setText(tr("Stop"));
	}
	else
	{
		qDebug() << QString("Play");
		ui->pushButton_2->setText(tr("Play"));
	}
}