#ifndef __INFINITE_COUNT_WORKER_H__
#define __INFINITE_COUNT_WORKER_H__

#include <QObject>

class HiloUI : public QObject
{
    Q_OBJECT

    public:
        HiloUI();

    public slots:
        void doWork();
        void stopWork();

    signals:
        void updateCount(int);
        void finished();

    private:
        bool m_running;
};

#endif // __INFINITE_COUNT_WORKER_H__
