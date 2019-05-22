#ifndef __HILO_REPARTIDOR_H__
#define __HILO_REPARTIDOR_H__

#include <QObject>

class HiloRepartidor : public QObject
{
    Q_OBJECT

    public:
        HiloRepartidor(int hilo, int speed, int start, int end, bool esperando);

    public slots:
        void doWork();
        void stopWork();
        void velocidad(int);
        void enEspera(bool);
        bool isEsperando();

    signals:
        void updateRepartidorTiempo(int, int);
        void finished();

    private:
        int m_hilo;
        unsigned int m_hiloSpeed;
        int m_hiloStart;
        int m_hiloEnd;
        bool m_running;
        bool m_esperando;
};

#endif // __INFINITE_COUNT_WORKER_H__
