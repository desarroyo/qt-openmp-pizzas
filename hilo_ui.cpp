#include <QApplication>

#include "hilo_ui.h"
#include "portablesleep.h"

HiloUI::HiloUI()
    : m_running(true)
{
}

void HiloUI::doWork()
{
    int i = 0;
    while (m_running) {
        emit updateCount(i);
        i++;
        qApp->processEvents();
        PortableSleep::msleep(1000);
    }
    emit finished();
}

void HiloUI::stopWork()
{
    m_running = false;
}
