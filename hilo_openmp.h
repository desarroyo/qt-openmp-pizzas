#ifndef __HILO_OPENMP_H__
#define __HILO_OPENMP_H__

#include <QObject>
#include <omp.h>

class HiloOpenMP : public QObject
{
    Q_OBJECT

    public:
        HiloOpenMP(int hilo, int pedidos, bool esperando);

    public slots:
        void doWork();
        void stopWork();
        void pedidos(int);
        void enEspera(bool);
        bool isEsperando();

    signals:
        void updateOpenMPTiempo(int, int);
        void finished();

    private:
        int m_hilo;
        unsigned int m_pedidos = 0;
        bool m_running;
        bool m_esperando;
};

#endif // __INFINITE_COUNT_WORKER_H__
