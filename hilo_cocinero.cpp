#include <QApplication>

#include "hilo_cocinero.h"
#include "portablesleep.h"


HiloCocinero::HiloCocinero(int hilo, int velocidad, int start, int end, bool esperando)
    : m_running(true)
{
    m_hilo      = hilo;
    m_hiloStart = start;
    m_hiloEnd   = end;
    m_hiloSpeed = velocidad;
    m_esperando = esperando;
}

void HiloCocinero::velocidad(int velocidad)
{
    m_hiloSpeed = velocidad;
}

void HiloCocinero::enEspera(bool esperando)
{
    m_esperando = esperando;
}

bool HiloCocinero::isEsperando()
{
    return m_esperando;
}

void HiloCocinero::doWork()
{
    int i = 0;
    while (m_running) {
        if(!m_esperando){
            emit updateCocineroTiempo(i, m_hilo);
            i++;
            qApp->processEvents();
            PortableSleep::msleep(m_hiloSpeed);
        }else{
            PortableSleep::msleep(500);
        }
    }
    emit finished();
}

void HiloCocinero::stopWork()
{
    m_running = false;
}
