#include <QApplication>

#include "hilo_openmp.h"
#include "portablesleep.h"
#include <omp.h>
#include <QList>
#include <unistd.h>


int const _TIEMPO_MAX_PREPARACION = 4;
int const _NUM_COCINEROS = 3;
int last = 0;

HiloOpenMP::HiloOpenMP(int hilo, int pedidos, bool esperando)
    : m_running(true)
{
    m_hilo      = hilo;
    m_pedidos = pedidos;
    m_esperando = esperando;

}

void HiloOpenMP::pedidos(int pedidos)
{
    last =  m_pedidos;
    m_pedidos = m_pedidos + pedidos;
}

void HiloOpenMP::enEspera(bool esperando)
{
    PortableSleep::msleep(1000);
    m_esperando = esperando;
}

bool HiloOpenMP::isEsperando()
{
    return m_esperando;
}

void HiloOpenMP::doWork()
{
    int i = 0;

    omp_lock_t lock;
    omp_init_lock(&lock);

    while (m_running) {
        if(!m_esperando){


            int semaphore_count  = 0;

#pragma omp parallel for
            for(int thread=0; thread<_NUM_COCINEROS; thread++)
            {

                for(int p=thread+last; p < m_pedidos; p+=_NUM_COCINEROS)
                {
                    omp_set_lock(&lock);

                    //qDebug("Soy el hilo %d en el pedido %d\n",thread,p);
                    emit updateOpenMPTiempo(p+1, thread+1);
                    semaphore_count++;

                    if(semaphore_count >= _NUM_COCINEROS){
                        PortableSleep::msleep(1000 * (_TIEMPO_MAX_PREPARACION+1));
                        semaphore_count= 0;
                    }

                    omp_unset_lock(&lock);
                }
            } // Implicit barrier at the end of the parallel for
#pragma omp barrier
            // Why a barrier when there is only one thread?



            enEspera(true);

        }else{
            PortableSleep::msleep(500);
        }
    }

    omp_destroy_lock(&lock);
    emit finished();
}

void HiloOpenMP::stopWork()
{
    m_running = false;
}
