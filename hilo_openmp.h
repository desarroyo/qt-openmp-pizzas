#ifndef __HILO_OPENMP_H__
#define __HILO_OPENMP_H__

#include <QObject>
#include <omp.h>

class HiloOpenMP : public QObject
{
    Q_OBJECT

    public:
        HiloOpenMP(int hilo, int speed, int start, int end, bool esperando);

    public slots:
        void doWork();
        void stopWork();
        void velocidad(int);
        void enEspera(bool);
        bool isEsperando();

    signals:
        void updateOpenMPTiempo(int, int);
        void finished();

    private:
        int m_hilo;
        unsigned int m_hiloSpeed;
        int m_hiloStart;
        int m_hiloEnd;
        bool m_running;
        bool m_esperando;
        omp_lock_t hacer_pizza1;
        omp_lock_t hacer_pizza2;
        omp_lock_t hacer_pizza3;
};

#endif // __INFINITE_COUNT_WORKER_H__
