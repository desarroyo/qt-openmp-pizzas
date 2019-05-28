#include <QMessageBox>
#include <QThread>
#include <QString>
#include <QDir>

#include "mainwindow.h"
#include "hilo_ui.h"
#include "hilo_repartidor.h"
#include "hilo_cocinero.h"
#include "hilo_openmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <QApplication>
#include <QMetaObject>
#include <qobjectdefs.h>
#include <QStringList>
#include <QDebug>
#include <omp.h>
#include <QDebug>
#include "portablesleep.h"
#include <QTime>
#include <QLineEdit>
#include <QVector>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif


HiloRepartidor *repartidor1;
HiloRepartidor *repartidor2;

HiloCocinero *cocinero1;
HiloCocinero *cocinero2;
HiloCocinero *cocinero3;

HiloOpenMP *openMP1;

int const _NUM_COCINEROS = 3;

int const _IMG_REPARTIDOR_1_POS_INI = 250;

int const _TIEMPO_MIN_PREPARACION = 1;
int const _TIEMPO_MAX_PREPARACION = 4;

int const _TIEMPO_MIN_REPARTIDOR = 5;
int const _TIEMPO_MAX_REPARTIDOR = 9;

int const _TIEMPO_GENERACION_PEDIDOS = 10;
int const _MAX_PEDIDOS_A_LA_VEZ = 3;

int contRepartidor1 = 0;
int contRepartidor2 = 0;

int contCocinero1 = 0;
int contCocinero2 = 0;
int contCocinero3 = 0;

int p1 = -1;
int p2 = -1;
int p3 = -1;

int r1 = -1;
int r2 = -1;

int contPedido = 0;


QList<int> listPedidos;
QList<int> alListas;
omp_lock_t hacer_pizza1;

bool prepararPizzas = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setFixedSize(1218, 729);

    /*
    * Prevent the warning: QObject::connect: Cannot queue arguments of type
    * 'QVector<int>'
    */
    qRegisterMetaType<QVector<int>>();


    ui.setupUi(this);

    ui.lblCount->setText(QString::number(0)+" seg.");
    ui.txtPedidos->setValidator( new QIntValidator(1, 100, this) );


    startRepartidores();
    startCocineros();

    inicializaLista();
    startInfiniteCount();
    startOpenMP();
}


void MainWindow::updateOpenMPTiempo(int pedido, int hilo){
    //qDebug("OPENMP");

    preparaPizza(pedido, hilo);
}

void MainWindow::preparaPizza(int pedido, int hilo){
    qDebug("Cocinero %d / pedido #%d\n",hilo,pedido);

    switch (hilo) {
    case 1:

        if(cocinero1->isEsperando()){
            p1 = getSiguientePedido(pedido);
            qDebug("Fila %d / pedido #%d\n",p1,pedido);
            if(p1 > -1){
                QString txtPedido = ui.tblPedidos->item(p1, 0)->text();
                on_btnCocinero1_clicked();
                ui.lblNoPedido1->setText(txtPedido);
                ui.tblPedidos->selectRow(p1);
            }

        }else{
            ui.lblNoPedido1->setText("-");
            ui.pizza01->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));
        }
        break;
    case 2:
        if(cocinero2->isEsperando()){
            p2 = getSiguientePedido(pedido);
            qDebug("Fila %d / pedido #%d\n",p2,pedido);
            if(p2 > -1){
                QString txtPedido = ui.tblPedidos->item(p2, 0)->text();
                on_btnCocinero2_clicked();
                ui.lblNoPedido2->setText(txtPedido);
                ui.tblPedidos->selectRow(p2);
            }else{
                ui.lblNoPedido2->setText("-");
                ui.pizza02->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));
            }
        }
        break;
    case 3:
        if(cocinero3->isEsperando()){
            p3 = getSiguientePedido(pedido);
            qDebug("Fila %d / pedido #%d\n",p3,pedido);
            if(p3 > -1){
                QString txtPedido = ui.tblPedidos->item(p3, 0)->text();
                on_btnCocinero3_clicked();
                ui.lblNoPedido3->setText(txtPedido);
                ui.tblPedidos->selectRow(p3);
            }else{
                ui.lblNoPedido3->setText("-");
                ui.pizza03->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));
            }
        }
        break;
    }
}

void MainWindow::updateCocineroTiempo(int cnt, int hilo){

    int porc_pizza = 0;

    switch (hilo) {
    case 1:

        ui.pbCocinero01->setValue(contCocinero1);

        porc_pizza = (int) (contCocinero1 * 10 ) / ui.pbCocinero01->maximum();

        //qDebug("Tiempo Pizza: %s", qUtf8Printable(QString::number(porc_pizza)));

        ui.pizza01->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(porc_pizza*10)+".png"));

        if(contCocinero1 == ui.pbCocinero01->maximum()){
            cocinero1->enEspera(true);

            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(p1, 1);
            pCellEstatus->setText("L");
            ui.tblPedidos->selectRow(p1);

            alListas.append(p1);
            ui.lblNoListos->setText(QString::number(countListos()));

            p1 = -1;
        }

        contCocinero1 = contCocinero1+1;

        break;
    case 2:

        ui.pbCocinero02->setValue(contCocinero2);

        porc_pizza = (int) (contCocinero2 * 10 ) / ui.pbCocinero02->maximum();

        //qDebug("Tiempo Pizza: %s", qUtf8Printable(QString::number(porc_pizza)));

        ui.pizza02->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(porc_pizza*10)+".png"));

        if(contCocinero2 == ui.pbCocinero02->maximum()){
            cocinero2->enEspera(true);

            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(p2, 1);
            pCellEstatus->setText("L");
            ui.tblPedidos->selectRow(p2);

            alListas.append(p2);
            ui.lblNoListos->setText(QString::number(countListos()));

            p2 = -1;

        }

        contCocinero2 = contCocinero2+1;

        break;

    case 3:

        ui.pbCocinero03->setValue(contCocinero3);

        porc_pizza = (int) (contCocinero3 * 10 ) / ui.pbCocinero03->maximum();

        //qDebug("Tiempo Pizza: %s", qUtf8Printable(QString::number(porc_pizza)));

        ui.pizza03->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(porc_pizza*10)+".png"));

        if(contCocinero3 == ui.pbCocinero03->maximum()){
            cocinero3->enEspera(true);

            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(p3, 1);
            pCellEstatus->setText("L");
            ui.tblPedidos->selectRow(p3);

            alListas.append(p3);
            ui.lblNoListos->setText(QString::number(countListos()));

            p3 = -1;

        }

        contCocinero3 = contCocinero3+1;

        break;
    }

}

void MainWindow::on_btnCocinero1_clicked()
{

    contCocinero1 = 0;
    int tiempoPreparacion = qrand() % ((_TIEMPO_MAX_PREPARACION+ 1) - _TIEMPO_MIN_PREPARACION) + _TIEMPO_MIN_PREPARACION;

    //qDebug("Tiempo: %s", qUtf8Printable(QString::number(tiempoPreparacion)));

    ui.pbCocinero01->setMaximum(tiempoPreparacion);
    ui.pbCocinero01->setValue(contCocinero1);

    ui.pizza01->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));


    cocinero1->enEspera(false);


}

void MainWindow::on_btnCocinero2_clicked()
{

    qDebug(" Cocinero 2");
    contCocinero2 = 0;
    int tiempoPreparacion = qrand() % ((_TIEMPO_MAX_PREPARACION+ 1) - _TIEMPO_MIN_PREPARACION) + _TIEMPO_MIN_PREPARACION;

    //qDebug("Tiempo: %s", qUtf8Printable(QString::number(tiempoPreparacion)));

    ui.pbCocinero02->setMaximum(tiempoPreparacion);
    ui.pbCocinero02->setValue(contCocinero2);

    ui.pizza02->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));


    cocinero2->enEspera(false);
}

void MainWindow::on_btnCocinero3_clicked()
{

    contCocinero3 = 0;
    int tiempoPreparacion = qrand() % ((_TIEMPO_MAX_PREPARACION+ 1) - _TIEMPO_MIN_PREPARACION) + _TIEMPO_MIN_PREPARACION;

    //qDebug("Tiempo: %s", qUtf8Printable(QString::number(tiempoPreparacion)));

    ui.pbCocinero03->setMaximum(tiempoPreparacion);
    ui.pbCocinero03->setValue(contCocinero3);

    ui.pizza03->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));


    cocinero3->enEspera(false);

}

void MainWindow::updateRepartidorTiempo(int cnt, int hilo){
    switch (hilo) {
    case 1:
        ui.imgRepartidor1->setGeometry(ui.imgRepartidor1->geometry().x()+(contRepartidor1*130),ui.imgRepartidor1->geometry().y(),ui.imgRepartidor1->geometry().width(),ui.imgRepartidor1->geometry().height());
        ui.pbRepartidor01->setValue(contRepartidor1);

        if(ui.pbRepartidor01->value() == ui.pbRepartidor01->maximum()-1){
            ui.imgRepartidor1->setPixmap(ui.imgRepartidor1->pixmap()->transformed(QTransform().scale(-1, 1)));

            ui.imgRepartidor1->setGeometry(800,ui.imgRepartidor1->geometry().y(),ui.imgRepartidor1->geometry().width(),ui.imgRepartidor1->geometry().height());
        }else if(ui.pbRepartidor01->value() == ui.pbRepartidor01->maximum()){
            ui.imgRepartidor1->setGeometry(_IMG_REPARTIDOR_1_POS_INI,ui.imgRepartidor1->geometry().y(),ui.imgRepartidor1->geometry().width(),ui.imgRepartidor1->geometry().height());

            ui.imgRepartidor1->setPixmap(ui.imgRepartidor1->pixmap()->transformed(QTransform().scale(-1, 1)));

            repartidor1->enEspera(true);

            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(r1, 1);
            pCellEstatus->setText("-");
            ui.tblPedidos->selectRow(r1);

            //alListas.removeAt(r1);
            ui.lblNoListos->setText(QString::number(countListos()));
            ui.lblNoPedidosCompletos->setText(QString::number(countCompletos()));

            r1 = -1;

        }
        contRepartidor1 = contRepartidor1+1;
        break;
    case 2:
        ui.imgRepartidor2->setGeometry(ui.imgRepartidor2->geometry().x()+(contRepartidor2*130),ui.imgRepartidor2->geometry().y(),ui.imgRepartidor2->geometry().width(),ui.imgRepartidor2->geometry().height());
        ui.pbRepartidor02->setValue(contRepartidor2);

        if(ui.pbRepartidor02->value() == ui.pbRepartidor02->maximum()-1){
            ui.imgRepartidor2->setPixmap(ui.imgRepartidor2->pixmap()->transformed(QTransform().scale(-1, 1)));

            ui.imgRepartidor2->setGeometry(800,ui.imgRepartidor2->geometry().y(),ui.imgRepartidor2->geometry().width(),ui.imgRepartidor2->geometry().height());
        }else if(ui.pbRepartidor02->value() == ui.pbRepartidor02->maximum()){
            ui.imgRepartidor2->setGeometry(_IMG_REPARTIDOR_1_POS_INI,ui.imgRepartidor2->geometry().y(),ui.imgRepartidor2->geometry().width(),ui.imgRepartidor2->geometry().height());

            ui.imgRepartidor2->setPixmap(ui.imgRepartidor2->pixmap()->transformed(QTransform().scale(-1, 1)));

            repartidor2->enEspera(true);

            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(r2, 1);
            pCellEstatus->setText("-");
            ui.tblPedidos->selectRow(r2);

            //alListas.removeAt(r2);
            ui.lblNoListos->setText(QString::number(countListos()));
            ui.lblNoPedidosCompletos->setText(QString::number(countCompletos()));

            r2 = -1;
        }
        contRepartidor2 = contRepartidor2+1;
        break;
    }

}

void MainWindow::on_btnRepartidor1_clicked()
{

    qDebug(" Repartidor 1");
    contRepartidor1 = 0;
    int tiempoRepartidor = qrand() % ((_TIEMPO_MAX_REPARTIDOR + 1) - _TIEMPO_MIN_REPARTIDOR) + _TIEMPO_MIN_REPARTIDOR;

    qDebug("Tiempo: %s", qUtf8Printable(QString::number(tiempoRepartidor)));

    ui.pbRepartidor01->setMaximum(tiempoRepartidor);
    ui.pbRepartidor01->setValue(contRepartidor1);

    QPixmap pixmap_reflect = ui.imgRepartidor1->pixmap()->transformed(QTransform().scale(1, 1));
    ui.imgRepartidor1->setPixmap(pixmap_reflect);

    ui.imgRepartidor1->setGeometry(_IMG_REPARTIDOR_1_POS_INI,ui.imgRepartidor1->geometry().y(),ui.imgRepartidor1->geometry().width(),ui.imgRepartidor1->geometry().height());



    repartidor1->enEspera(false);


}

void MainWindow::on_btnRepartidor2_clicked()
{

    qDebug(" Repartidor 2");
    contRepartidor2 = 0;
    int tiempoRepartidor = qrand() % ((_TIEMPO_MAX_REPARTIDOR + 1) - _TIEMPO_MIN_REPARTIDOR) + _TIEMPO_MIN_REPARTIDOR;

    qDebug("Tiempo: %s", qUtf8Printable(QString::number(tiempoRepartidor)));

    ui.pbRepartidor02->setMaximum(tiempoRepartidor);
    ui.pbRepartidor02->setValue(contRepartidor2);

    QPixmap pixmap_reflect = ui.imgRepartidor2->pixmap()->transformed(QTransform().scale(1, 1));
    ui.imgRepartidor2->setPixmap(pixmap_reflect);

    ui.imgRepartidor2->setGeometry(_IMG_REPARTIDOR_1_POS_INI,ui.imgRepartidor2->geometry().y(),ui.imgRepartidor2->geometry().width(),ui.imgRepartidor2->geometry().height());



    repartidor2->enEspera(false);

}

void MainWindow::startOpenMP()
{
    QThread  *openMPHilo1;
    openMPHilo1 = new QThread;
    openMP1 = new HiloOpenMP(1, 0, true);

    openMP1->moveToThread(openMPHilo1);


    connect(openMPHilo1, SIGNAL(started()), openMP1, SLOT(doWork()));
    connect(openMP1, SIGNAL(finished()), openMPHilo1, SLOT(quit()));

    connect(openMP1, SIGNAL(finished()), openMP1, SLOT(deleteLater()));

    //connect(openMP1, SIGNAL(finished()), this, SLOT(countFinished()));
    //connect(openMP2, SIGNAL(finished()), this, SLOT(countFinished()));
    //connect(openMP3, SIGNAL(finished()), this, SLOT(countFinished()));


    connect(openMPHilo1, SIGNAL(finished()), openMPHilo1, SLOT(deleteLater()));

    connect(openMP1, SIGNAL(updateOpenMPTiempo(int, int)), this, SLOT(updateOpenMPTiempo(int, int)));

    //connect(ui.btnStop, SIGNAL(clicked()), openMP1, SLOT(stopWork()));

    openMPHilo1->start();


}

void MainWindow::startRepartidores()
{
    QThread  *repartidorHilo1;
    QThread  *repartidorHilo2;

    repartidorHilo1 = new QThread;
    repartidorHilo2 = new QThread;

    repartidor1 = new HiloRepartidor(1, 1000, 0, 9, true);
    repartidor2 = new HiloRepartidor(2, 1000, 0, 9, true);


    repartidor1->moveToThread(repartidorHilo1);
    repartidor2->moveToThread(repartidorHilo2);


    connect(repartidorHilo1, SIGNAL(started()), repartidor1, SLOT(doWork()));
    connect(repartidorHilo2, SIGNAL(started()), repartidor2, SLOT(doWork()));
    connect(repartidor1, SIGNAL(finished()), repartidorHilo1, SLOT(quit()));
    connect(repartidor2, SIGNAL(finished()), repartidorHilo2, SLOT(quit()));

    connect(repartidor1, SIGNAL(finished()), repartidor1, SLOT(deleteLater()));
    connect(repartidor2, SIGNAL(finished()), repartidor2, SLOT(deleteLater()));

    //connect(repartidor1, SIGNAL(finished()), this, SLOT(countFinished()));
    //connect(repartidor2, SIGNAL(finished()), this, SLOT(countFinished()));

    connect(repartidorHilo1, SIGNAL(finished()), repartidorHilo1, SLOT(deleteLater()));
    connect(repartidorHilo2, SIGNAL(finished()), repartidorHilo2, SLOT(deleteLater()));

    connect(repartidor1, SIGNAL(updateRepartidorTiempo(int, int)), this, SLOT(updateRepartidorTiempo(int, int)));
    connect(repartidor2, SIGNAL(updateRepartidorTiempo(int, int)), this, SLOT(updateRepartidorTiempo(int, int)));

    //connect(ui.btnStop, SIGNAL(clicked()), repartidor1, SLOT(stopWork()));
    //connect(ui.btnStop, SIGNAL(clicked()), repartidor2, SLOT(stopWork()));

    repartidorHilo1->start();
    repartidorHilo2->start();


}

void MainWindow::startCocineros()
{
    QThread  *cocineroHilo1;
    QThread  *cocineroHilo2;
    QThread  *cocineroHilo3;

    cocineroHilo1 = new QThread;
    cocineroHilo2 = new QThread;
    cocineroHilo3 = new QThread;

    cocinero1 = new HiloCocinero(1, 1000, 0, 4, true);
    cocinero2 = new HiloCocinero(2, 1000, 0, 4, true);
    cocinero3 = new HiloCocinero(3, 1000, 0, 4, true);


    cocinero1->moveToThread(cocineroHilo1);
    cocinero2->moveToThread(cocineroHilo2);
    cocinero3->moveToThread(cocineroHilo3);



    connect(cocineroHilo1, SIGNAL(started()), cocinero1, SLOT(doWork()));
    connect(cocineroHilo2, SIGNAL(started()), cocinero2, SLOT(doWork()));
    connect(cocineroHilo3, SIGNAL(started()), cocinero3, SLOT(doWork()));

    connect(cocinero1, SIGNAL(finished()), cocineroHilo1, SLOT(quit()));
    connect(cocinero2, SIGNAL(finished()), cocineroHilo2, SLOT(quit()));
    connect(cocinero3, SIGNAL(finished()), cocineroHilo3, SLOT(quit()));


    connect(cocinero1, SIGNAL(finished()), cocinero1, SLOT(deleteLater()));
    connect(cocinero2, SIGNAL(finished()), cocinero2, SLOT(deleteLater()));
    connect(cocinero3, SIGNAL(finished()), cocinero3, SLOT(deleteLater()));


    //connect(cocinero1, SIGNAL(finished()), this, SLOT(countFinished()));
    //connect(cocinero2, SIGNAL(finished()), this, SLOT(countFinished()));
    //connect(cocinero3, SIGNAL(finished()), this, SLOT(countFinished()));


    connect(cocineroHilo1, SIGNAL(finished()), cocineroHilo1, SLOT(deleteLater()));
    connect(cocineroHilo2, SIGNAL(finished()), cocineroHilo2, SLOT(deleteLater()));
    connect(cocineroHilo3, SIGNAL(finished()), cocineroHilo3, SLOT(deleteLater()));


    connect(cocinero1, SIGNAL(updateCocineroTiempo(int, int)), this, SLOT(updateCocineroTiempo(int, int)));
    connect(cocinero2, SIGNAL(updateCocineroTiempo(int, int)), this, SLOT(updateCocineroTiempo(int, int)));
    connect(cocinero3, SIGNAL(updateCocineroTiempo(int, int)), this, SLOT(updateCocineroTiempo(int, int)));


    //connect(ui.btnStop, SIGNAL(clicked()), cocinero1, SLOT(stopWork()));
    //connect(ui.btnStop, SIGNAL(clicked()), cocinero2, SLOT(stopWork()));
    //connect(ui.btnStop, SIGNAL(clicked()), cocinero3, SLOT(stopWork()));


    cocineroHilo1->start();
    cocineroHilo2->start();
    cocineroHilo3->start();



}

void MainWindow::inicializaLista()
{
    contPedido = 1;

    ui.tblPedidos->setRowCount(0);
    ui.tblPedidos->clear();
    ui.tblPedidos->setHorizontalHeaderLabels( {"#", "Est"} );
    ui.tblPedidos->setColumnWidth(0,15);
    ui.tblPedidos->setColumnWidth(1,10);

}

void MainWindow::on_btnCrearPedido_clicked()
{
    int noPedidos = 0;
    try {
        noPedidos = ui.txtPedidos->text().toInt();
    } catch (...) {

    }

    if(noPedidos> 0){
        ui.btnCrearPedido->setEnabled(false);
        openMP1->pedidos(noPedidos);

        generaPedidos(noPedidos);

        openMP1->enEspera(false);


        prepararPizzas = true;

    }



}

void MainWindow::generaPedidos(int noPedidos)
{

    contPedido = ui.tblPedidos->rowCount()+1;
#pragma omp parallel for num_threads(_NUM_COCINEROS)
    for ( int i = contPedido; i < contPedido+noPedidos; i++ )
    {
#pragma omp critical
        agregaPedido(i);
    }


}

void MainWindow::preparandoPedido(int pedido)
{
    int fila = pedido-1;

    QTableWidgetItem *pCell = ui.tblPedidos->item(fila, 0);
    pCell->setText("#"+QString::number(pedido)+ " / Prep...");

    ui.lblNoPedidos->setText( QString::number(ui.tblPedidos->rowCount() ) );
    ui.tblPedidos->scrollTo(ui.tblPedidos->model()->index(fila, 0));
    ui.tblPedidos->selectRow(fila);
}

void MainWindow::agregaPedido()
{

    int fila = ui.tblPedidos->rowCount();
    ui.tblPedidos->insertRow( fila );

    QTableWidgetItem *pCell = ui.tblPedidos->item(fila, 0);
    if(!pCell)
    {
        pCell = new QTableWidgetItem;
        ui.tblPedidos->setItem(fila, 0, pCell);
    }
    pCell->setText("#"+QString::number(++contPedido));

    ui.lblNoPedidos->setText( QString::number(fila+1 ) );

}

void MainWindow::agregaPedido(int noPedido)
{

    int fila = ui.tblPedidos->rowCount();
    ui.tblPedidos->insertRow( fila );

    QTableWidgetItem *pCellPedido = ui.tblPedidos->item(fila, 0);
    QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(fila, 0);
    if(!pCellPedido)
    {
        pCellPedido = new QTableWidgetItem;
        ui.tblPedidos->setItem(fila, 0, pCellPedido);
    }
    if(!pCellEstatus)
    {
        pCellEstatus = new QTableWidgetItem;
        ui.tblPedidos->setItem(fila, 1, pCellEstatus);
    }
    pCellPedido->setText("#"+QString::number(noPedido));
    pCellEstatus->setText("E");

    ui.lblNoPedidos->setText( QString::number(fila+1 ) );

}


int MainWindow::getSiguientePedido(){
    QString pedido = "";
    QString estatus = "";
    for (int fila = 0;fila< ui.tblPedidos->rowCount();fila++) {
        pedido = ui.tblPedidos->item(fila, 0)->text();
        estatus = ui.tblPedidos->item(fila, 1)->text();

        if(estatus == "E"){
            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(fila, 1);
            pCellEstatus->setText("P");
            return fila;
        }
    }

    return -1;
}

int MainWindow::getSiguientePedido(int p){
    QString pedido = "";
    QString estatus = "";

    for (int fila = 0;fila< ui.tblPedidos->rowCount();fila++) {
        pedido = ui.tblPedidos->item(fila, 0)->text();
        estatus = ui.tblPedidos->item(fila, 1)->text();

        if(pedido == "#"+QString::number(p)){
            qDebug("p %d vs %s est %s\n",p,qUtf8Printable(pedido), qUtf8Printable(estatus) );
        }
        if(estatus == "E" && QString::compare(pedido,"#"+QString::number(p) ) == 0){
            qDebug("pedido %d\n",p);
            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(fila, 1);
            pCellEstatus->setText("P");
            return fila;
        }
    }

    return -1;
}

int MainWindow::countListos(){
    int cont = 0;
    for (int i = 0; i< alListas.length(); i++) {
        QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(i, 1);
        if(pCellEstatus->text() == "L"){
            cont++;
        }
    }
    return cont;
}

int MainWindow::countPorPreparar(){
    int cont = 0;
    for (int i = 0; i< alListas.length(); i++) {
        QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(i, 1);
        if(pCellEstatus->text() == "E"){
            cont++;
        }
    }
    return cont;
}

int MainWindow::countCompletos(){
    int cont = 0;
    for (int i = 0; i< alListas.length(); i++) {
        QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(i, 1);
        if(pCellEstatus->text() == "-"){
            cont++;
        }
    }
    return cont;
}

int MainWindow::getSiguientePedidoRepartidor(){
    QString pedido = "";
    QString estatus = "";
    for (int fila = 0;fila< ui.tblPedidos->rowCount();fila++) {
        pedido = ui.tblPedidos->item(fila, 0)->text();
        estatus = ui.tblPedidos->item(fila, 1)->text();

        if(estatus == "L"){
            QTableWidgetItem *pCellEstatus = ui.tblPedidos->item(fila, 1);
            pCellEstatus->setText("R");
            return fila;
        }
    }

    return -1;
}

void MainWindow::updateInfiniteCount(int contador){
    //ui.lblTiempoTranscurrido->setText(QString::number(cnt));
    ui.lblCount->setText(QString::number(contador)+" seg.");


    if(prepararPizzas){
        if(cocinero1->isEsperando()){
            ui.lblNoPedido1->setText("-");
            ui.pizza01->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));
        }

        if(cocinero2->isEsperando()){
            ui.lblNoPedido2->setText("-");
            ui.pizza02->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));
        }

        if(cocinero3->isEsperando()){
            ui.lblNoPedido3->setText("-");
            ui.pizza03->setPixmap(QPixmap("assets/pizza/pizza_"+QString::number(0)+".png"));
        }


        if(alListas.length() > 0){
            if(repartidor1->isEsperando()){
                r1 = getSiguientePedidoRepartidor();
                if(r1 > -1){
                    QString pedido = ui.tblPedidos->item(r1, 0)->text();
                    on_btnRepartidor1_clicked();
                    ui.lblPedidoRepartiendo1->setText(pedido);
                }else{
                    ui.lblPedidoRepartiendo1->setText("-");
                }
            }
            if(repartidor2->isEsperando()){
                r2 = getSiguientePedidoRepartidor();
                if(r2 > -1){
                    QString pedido = ui.tblPedidos->item(r2, 0)->text();
                    on_btnRepartidor2_clicked();
                    ui.lblPedidoRepartiendo2->setText(pedido);
                }else{
                    ui.lblPedidoRepartiendo2->setText("-");
                }
            }
        }

        if(!openMP1->isEsperando() && !cocinero1->isEsperando() && !cocinero2->isEsperando() && !cocinero3->isEsperando()){
            openMP1->enEspera(true);
        }else if(openMP1->isEsperando() && cocinero1->isEsperando() && cocinero2->isEsperando() && cocinero3->isEsperando()){
            openMP1->enEspera(false);
        }

    }

    if(alListas.length() > 0  && openMP1->isEsperando()){
        if(countPorPreparar() == 0){
            ui.btnCrearPedido->setEnabled(true);
        }else {
            ui.btnCrearPedido->setEnabled(false);
        }
    }

}

void MainWindow::startInfiniteCount()
{
    QThread *uiThread;
    HiloUI *worker;

    uiThread = new QThread;
    worker   = new HiloUI;
    worker->moveToThread(uiThread);
    connect(uiThread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(finished()), uiThread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(uiThread, SIGNAL(finished()), uiThread, SLOT(deleteLater()));
    //connect(worker, SIGNAL(finished()), this, SLOT(infiniteCountFinished()));
    connect(worker, SIGNAL(updateCount(int)), this, SLOT(updateInfiniteCount(int)));
    //connect(ui.btnStop, SIGNAL(clicked()), worker, SLOT(stopWork()));
    uiThread->start();

}




