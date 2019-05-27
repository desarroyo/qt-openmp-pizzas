#include <QApplication>

#include "hilo_repartidor.h"
#include "portablesleep.h"


HiloRepartidor::HiloRepartidor(int hilo, int velocidad, int start, int end, bool esperando)
    : m_running(true)
{
    m_hilo      = hilo;
    m_hiloStart = start;
    m_hiloEnd   = end;
    m_hiloSpeed = velocidad;
    m_esperando = esperando;
}

void HiloRepartidor::velocidad(int velocidad)
{
    m_hiloSpeed = velocidad;
}

void HiloRepartidor::enEspera(bool esperando)
{
    m_esperando = esperando;
}

bool HiloRepartidor::isEsperando()
{
    return m_esperando;
}

void HiloRepartidor::doWork()
{
    int i = 0;
    while (m_running) {
        if(!m_esperando){
            emit updateRepartidorTiempo(i, m_hilo);
            i++;
            qApp->processEvents();
            PortableSleep::msleep(m_hiloSpeed);
        }else{
            PortableSleep::msleep(500);
        }
    }
    emit finished();
}

void HiloRepartidor::stopWork()
{
    m_running = false;
}
