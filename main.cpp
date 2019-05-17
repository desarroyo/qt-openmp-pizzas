#include <QApplication>

#include <mainwindow.h>
#include <omp.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    int numhilos = 0;
    int hilos    = 0;

    // pragma indicar que es una nomenclatura tipo openmp
    // private => las variable definidas en ese ambiente, son excluivas
    //            por cierto proceso, y no es posible alterar su contenido
    //            por otro proceso diferente
    #pragma omp parallel private(numhilos, hilos)
    {
        //total de hebras que auxilian para efectuar un proceso
        numhilos = omp_get_num_threads();
        //total de hilos del CPU
        hilos    = omp_get_thread_num();
        printf("Soy la hebra %d de un total de %d\n",numhilos, hilos);
    }

    printf("Cuentas con %d hilos\n",hilos);

    MainWindow w;
    w.show();





    return app.exec();
}
