#include <QApplication>

#include "countworker.h"
#include "portablesleep.h"

PortableSleep::PortableSleep(int start, int end)
{
    m_countStart = start;
    m_countEnd   = end;
}

void PortableSleep::doWork()
{
    for (int i = m_countStart; i <= m_countEnd; i++) {
        emit updateCount(i);
        qApp->processEvents();
        PortableSleep::msleep(1000);
    }
    emit finished();
}
