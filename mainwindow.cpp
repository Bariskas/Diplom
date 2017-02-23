#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindowClass)
{
	myPlayer = new Player();
	QObject::connect(myPlayer, SIGNAL(processedImage(QImage)),
		this, SLOT(updatePlayerUI(QImage)));
	ui->setupUi(this);

	if (!myPlayer->loadVideo())
	{
		QMessageBox msgBox;
		msgBox.setText("Webcam not found");
		msgBox.exec();
	}

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
	if (myPlayer->isStopped())
	{
		myPlayer->Play();
		ui->pushButton_2->setText(tr("Stop"));
	}
	else
	{
		myPlayer->Stop();
		ui->pushButton_2->setText(tr("Play"));
	}
}