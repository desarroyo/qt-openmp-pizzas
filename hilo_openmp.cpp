#include <QApplication>

#include "hilo_openmp.h"
#include "portablesleep.h"
#include <omp.h>


HiloOpenMP::HiloOpenMP(int hilo, int velocidad, int start, int end, bool esperando)
    : m_running(true)
{
    m_hilo      = hilo;
    m_hiloStart = start;
    m_hiloEnd   = end;
    m_hiloSpeed = velocidad;
    m_esperando = esperando;

    omp_lock_t hacer_pizza1;
}

void HiloOpenMP::velocidad(int velocidad)
{
    m_hiloSpeed = velocidad;
}

void HiloOpenMP::enEspera(bool esperando)
{
    m_esperando = esperando;
}

bool HiloOpenMP::isEsperando()
{
    return m_esperando;
}

void HiloOpenMP::doWork()
{
    int i = 0;


    omp_init_lock(&hacer_pizza1);

    while (m_running) {
        if(!m_esperando){



            int pizzas_pendientes = 100;
            int contador = 1;

            int hilo = 0;
            QString orden = "";

            /*
            omp_set_lock(&hacer_pizza1);
            omp_set_lock(&hacer_pizza2);
            omp_set_lock(&hacer_pizza3);
            */

            #pragma omp parallel for num_threads(3)
            for ( int i = 1; i <= pizzas_pendientes; i++ )
            {

                #pragma omp critical
                {
                    // one thread at a time stuff


                    hilo = omp_get_thread_num()+1;
                    //qDebug("Soy la hebra %d de un total de %d\n",hilo,3);

                    orden = "#"+QString::number(i);
                    emit updateOpenMPTiempo(i, hilo);

                    if(hilo == 1){
                        //PortableSleep::msleep(4000);
                     omp_set_lock(&hacer_pizza1);
                    }else if(hilo == 2){
                         //PortableSleep::msleep(4000);
                     omp_set_lock(&hacer_pizza2);
                    }else if(hilo == 3){
                         //PortableSleep::msleep(4000);
                     omp_set_lock(&hacer_pizza3);
                    }

                   //orden = ui.tblPedidos->item(i, 0)->text();

                   //qDebug("Orden: %s del hilo %d", qUtf8Printable(orden), hilo);
                   //qDebug("%s", qUtf8Printable(QString::number(i)));







                }


                //i++;
                qApp->processEvents();


                /*
                #pragma omp critical
                {
               omp_unset_lock(&hacer_pizza1);
               omp_unset_lock(&hacer_pizza2);
               omp_unset_lock(&hacer_pizza3);
                }
                */

               //contador = contador+ 1;
               //ui.lblCount->setText(QString::number(contador)+" seg.");
            }

            omp_destroy_lock(&hacer_pizza1);
            omp_destroy_lock(&hacer_pizza2);
            omp_destroy_lock(&hacer_pizza3);

            enEspera(true);
        }
    }
    emit finished();
}

void HiloOpenMP::stopWork()
{
    m_running = false;
}
