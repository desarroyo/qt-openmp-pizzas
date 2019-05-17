#include <QMessageBox>
#include <QThread>
#include <QString>
#include <QDir>

#include "mainwindow.h"
#include "hilo_ui.h"
#include "hilo_abcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <QApplication>
#include <QMetaObject>
#include <qobjectdefs.h>
#include <QFile>
#include <QStringList>
#include <QDebug>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

int vel_a= 0;
int vel_b= 0;
int vel_c= 0;
int vel_d= 0;


int cont_a= 0;
int cont_b= 0;
int cont_c= 0;
int cont_d= 0;

int current_lectores= 0;
//int cont_lectores= 0;
//int cont_escritores= 0;

int lastId= 0;

HiloABCD *workerA;
HiloABCD *workerB;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      infiniteCountRunning(false),
      countRunning(false)
{

    setFixedSize(1218, 729);

    ui.setupUi(this);
    ui.lblLeyendo->setStyleSheet("QLabel { color : blue; }");
    ui.lblEscribiendo->setStyleSheet("QLabel { color : red; }");


    ui.lblHilo01_velocidad->setStyleSheet("QLabel { color : blue; }");
    ui.lblHilo02_velocidad->setStyleSheet("QLabel { color : red; }");

    ui.pbHilo01->setStyleSheet("QProgressBar {border: 2px solid grey;border-radius: 5px;background-color: #fbf3ec;}QProgressBar::chunk {background-color: #99d4d9;width: 20px;}");
    ui.pbHilo02->setStyleSheet("QProgressBar {border: 2px solid grey;border-radius: 5px;background-color: #fbf3ec;}QProgressBar::chunk {background-color: #daa482;width: 20px;}");

    vel_a = 1;
    ui.slLectores->setValue(vel_a);
    ui.pbHilo01->setValue(cont_a);

    vel_b = 0;
    ui.slEscritores->setValue(vel_b);
    ui.pbHilo02->setValue(cont_b);


    /*
    abrirCSV();
    appendCSV(101, "prueba");

    */

    connectSignalsSlots();
}

int MainWindow::appendCSV(int id, QString marca){
    QFile file("C:/_ARCHIVOS/QT/qt-pizzas/assets/datos.csv");


    if (file.open(QIODevice::Append)) {
        QString valor = ""+QString::number(id) + "," + marca + "\n";
        file.write(valor.toStdString().c_str());
        file.close();
     }

    abrirCSV();
}

int MainWindow::abrirCSV(){
    QString path = QDir::currentPath();

    printf("X\n");

    QFile file("C:/_ARCHIVOS/QT/qt-pizzas/assets/datos.csv");
       if (!file.open(QIODevice::ReadOnly)) {
           qDebug() << file.errorString();
           return 1;
       }


       ui.tblMarcas->setRowCount(0);
       ui.tblMarcas->clear();
       ui.tblMarcas->setHorizontalHeaderLabels( {"ID", "MARCA"} );

       QStringList wordList;
       int fila = 0;
       QTableWidgetItem pCell;
       while (!file.atEnd()) {
           fila = ui.tblMarcas->rowCount();
           ui.tblMarcas->insertRow( fila );
           QByteArray line = file.readLine();
           wordList.append(line.split(',').first());

           QTableWidgetItem *pCell = ui.tblMarcas->item(fila, 0);
           if(!pCell)
           {
               pCell = new QTableWidgetItem;
               ui.tblMarcas->setItem(fila, 0, pCell);
           }
           pCell->setText(line.split(',').first());

           pCell = ui.tblMarcas->item(fila, 1);
           if(!pCell)
           {
               pCell = new QTableWidgetItem;
               ui.tblMarcas->setItem(fila, 1, pCell);
           }
           pCell->setText(line.split(',').last());
           lastId = QString(line.split(',').first()).toInt();
       }
       file.close();

       //qDebug() << wordList;

       ui.tblMarcas->scrollToBottom();
}

void MainWindow::updateCount(int cnt, int hilo)
{
    switch (hilo) {
        case 1:
            cont_a = cont_a+1;
            ui.pbHilo01->setValue(cont_a);
            break;
    case 2:
        cont_b = cont_b+1;
        ui.pbHilo02->setValue(cont_b);
        break;
    }

    if(cont_a >= 100){
        workerA->enEspera(true);
        workerA->velocidad( (rand() %60)+30);
        vel_a = 0;
        current_lectores = 0;
        ui.slLectores->setValue(vel_a);
        ui.lblLeyendo->setText(QString::number(0)+ " estan leyendo ...");
        reiniciaLector();
    }

    if(cont_b >= 100){
        workerB->enEspera(true);
        lastId++;
        appendCSV(lastId, "Marca-"+QString::number(lastId));
        workerB->velocidad( (rand() %60)+10);
        ui.lblEscribiendo->setText(QString::number(0)+ " esta escribiendo ...");
        //vel_b = vel_b -1;
        //ui.slEscritores->setValue(vel_b);
        reiniciaEscritor();
    }

}

void MainWindow::reinicia(){
    cont_a= 0;
    ui.pbHilo01->setValue(cont_a);

    cont_b= 0;
    ui.pbHilo02->setValue(cont_b);

    ui.lblLeyendo->setText(QString::number(0)+ " estan leyendo ...");
    ui.lblEscribiendo->setText(QString::number(0)+ " esta escribiendo ...");

    abrirCSV();
}

void MainWindow::reiniciaLector(){
    cont_a= 0;
    ui.pbHilo01->setValue(cont_a);

}

void MainWindow::reiniciaEscritor(){
    cont_b= 0;
    ui.pbHilo02->setValue(cont_b);

}

void MainWindow::updateInfiniteCount(int cnt)
{
    ui.lblCount->setText(QString::number(cnt)+" seg.");
    //ui.pbHilo01->setValue(cont_a);
    ui.imgRepartidor1->setGeometry(ui.imgRepartidor1->geometry().x()+80,ui.imgRepartidor1->geometry().y(),ui.imgRepartidor1->geometry().width(),ui.imgRepartidor1->geometry().height());
    ui.imgRepartidor2->setGeometry(ui.imgRepartidor2->geometry().x()+200,ui.imgRepartidor2->geometry().y(),ui.imgRepartidor2->geometry().width(),ui.imgRepartidor2->geometry().height());

    ui.pizza01->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(cnt*10)+".png"));

    //ui.pizza01.show();
    if((rand() %100) > 75){
        vel_a = ui.slLectores->value() + (rand() %5);
        ui.slLectores->setValue(vel_a);
    }
    if((rand() %100) > 85){
        vel_b = ui.slEscritores->value() + (rand() %3);
        ui.slEscritores->setValue(vel_b);
    }




    if(workerB->isEsperando() && ui.slLectores->value() > 0 ){
            workerA->enEspera(false);
            workerB->enEspera(true);
            current_lectores = current_lectores + ui.slLectores->value();
            ui.lblLeyendo->setText(QString::number(current_lectores)+ " estan leyendo ...");
            vel_a = 0;
            ui.slLectores->setValue(vel_a);
     }
     else if(workerB->isEsperando() && workerA->isEsperando() && ui.slEscritores->value() > 0 && ui.slLectores->value() <= 0 ){
        workerB->enEspera(false);
        workerA->enEspera(true);
        ui.lblEscribiendo->setText(QString::number(1)+ " esta escribiendo ...");
        vel_b = vel_b -1;
        ui.slEscritores->setValue(vel_b);

    }
}



void MainWindow::startCount()
{
    reinicia();

    QThread  *workerThreadA;
    QThread  *workerThreadB;



    if (countRunning) {
        QMessageBox::critical(this, "Error", "Los Hilos ya estan corriendo!");
        return;
    }

    workerThreadA = new QThread;
    workerThreadB = new QThread;

    workerA       = new HiloABCD(1, (rand() %100)+50, 0, 100, true);
    workerB       = new HiloABCD(2, (rand() %100)+50, 0, 100, true);


    workerA->moveToThread(workerThreadA);
    workerB->moveToThread(workerThreadB);

    connect(workerThreadA, SIGNAL(started()), workerA, SLOT(doWork()));
    connect(workerThreadB, SIGNAL(started()), workerB, SLOT(doWork()));
    connect(workerA, SIGNAL(finished()), workerThreadA, SLOT(quit()));
    connect(workerB, SIGNAL(finished()), workerThreadB, SLOT(quit()));

    connect(workerA, SIGNAL(finished()), workerA, SLOT(deleteLater()));
    connect(workerB, SIGNAL(finished()), workerB, SLOT(deleteLater()));

    connect(workerA, SIGNAL(finished()), this, SLOT(countFinished()));
    connect(workerB, SIGNAL(finished()), this, SLOT(countFinished()));

    connect(workerThreadA, SIGNAL(finished()), workerThreadA, SLOT(deleteLater()));
    connect(workerThreadB, SIGNAL(finished()), workerThreadB, SLOT(deleteLater()));

    connect(workerA, SIGNAL(updateCount(int, int)), this, SLOT(updateCount(int, int)));
    connect(workerB, SIGNAL(updateCount(int, int)), this, SLOT(updateCount(int, int)));

    connect(ui.btnStop, SIGNAL(clicked()), workerA, SLOT(stopWork()));
    connect(ui.btnStop, SIGNAL(clicked()), workerB, SLOT(stopWork()));

    workerThreadA->start();
    workerThreadB->start();

    countRunning = true;
}

void MainWindow::startInfiniteCount()
{
    QThread             *workerThread;
    HiloUI *worker;

    if (infiniteCountRunning) {
        QMessageBox::critical(this, "Error", "El hilo de UI ya esta corriendo!");
        return;
    }

    workerThread = new QThread;
    worker       = new HiloUI;
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(finished()), workerThread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    connect(worker, SIGNAL(finished()), this, SLOT(infiniteCountFinished()));
    connect(worker, SIGNAL(updateCount(int)), this, SLOT(updateInfiniteCount(int)));
    connect(ui.btnStop, SIGNAL(clicked()), worker, SLOT(stopWork()));
    workerThread->start();

    infiniteCountRunning = true;

    /*
    pthread_t referencia;

    printf("Iniciando el proceso\n");

    struct arg_struct args;
       args.hilo = 1;
       args.vel = 100;
       args.mw = this;
       args.pb = this->ui.pbHilo01;

    pthread_create(&referencia, NULL, ejecutaSegundoPlano, (void *) &args);
    pthread_join(referencia,NULL);

    */
}

void MainWindow::countFinished()
{
    countRunning = false;

   reinicia();
}

void MainWindow::infiniteCountFinished()
{
    infiniteCountRunning = false;
    cont_a= 0;
    ui.pbHilo01->setValue(cont_a);
}

void MainWindow::connectSignalsSlots()
{
    connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(startInfiniteCount()));
    connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(startCount()));
}

void MainWindow::on_btnRandom_clicked()
{

    vel_a = rand() %12;
    ui.slLectores->setValue(vel_a);

    vel_b = rand() %10;
    ui.slEscritores->setValue(vel_b);


    actualizaVelocidadHilo();

}

void MainWindow::actualizaVelocidadHilo(){
    if(workerA){
        workerA->velocidad((rand() %80)+30);
    }
    if(workerB){
        workerB->velocidad((rand() %60)+10);
    }
}


void MainWindow::on_slLectores_valueChanged(int value)
{
    vel_a = value;
    ui.lblHilo01_velocidad->setText(QString::number(value));
    //actualizaVelocidadHilo();
}

void MainWindow::on_slEscritores_valueChanged(int value)
{
    vel_b = value;
    ui.lblHilo02_velocidad->setText(QString::number(value));
    //actualizaVelocidadHilo();
}

