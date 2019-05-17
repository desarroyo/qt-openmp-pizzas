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
        Q_SLOT void setEditText(const QString & str) {
            updateInfiniteCount(10);
          }

private slots:
        void updateCount(int cnt, int hilo);
        void updateInfiniteCount(int cnt);

        void startCount();
        void startInfiniteCount();

        void countFinished();
        void infiniteCountFinished();

        void on_btnRandom_clicked();

        void on_slLectores_valueChanged(int value);

        void on_slEscritores_valueChanged(int value);

        void reinicia();
        void reiniciaLector();
        void reiniciaEscritor();
        void actualizaVelocidadHilo();
        int abrirCSV();
        int appendCSV(int, QString);


    private:
        void connectSignalsSlots();

        bool countRunning;
        bool infiniteCountRunning;

        Ui_MainWindow ui;


};

#endif // __MAIN_WINDOW_H__
