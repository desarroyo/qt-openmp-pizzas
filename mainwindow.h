#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QMainWindow>
#include <QString>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);        

private slots:

        void updateInfiniteCount(int cnt);
        void updateRepartidorTiempo(int cnt, int hilo);
        void updateCocineroTiempo(int cnt, int hilo);
        void updateOpenMPTiempo(int cnt, int hilo);


        void startRepartidores();
        void startCocineros();
        void startOpenMP();

        void startInfiniteCount();


        void on_btnRepartidor1_clicked();
        void on_btnRepartidor2_clicked();

        void on_btnCocinero1_clicked();
        void on_btnCocinero2_clicked();
        void on_btnCocinero3_clicked();
        void on_btnCrearPedido_clicked();


        void on_btnStart_clicked();

        int getSiguientePedido();
        int getSiguientePedidoRepartidor();
        int countListos();
        int countCompletos();

        void inicializaLista();
        void agregaPedido();
        void agregaPedido(int);
        void preparandoPedido(int);
        void generaPedidos(int);
        void preparaPizzas();


    private:

        Ui_MainWindow ui;


};

#endif // __MAIN_WINDOW_H__
