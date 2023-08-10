#ifndef IMESSAGEQUEUEEVENTS_H
#define IMESSAGEQUEUEEVENTS_H

#include <iostream>
#include <functional>
#include <algorithm>
#include <list>

#include "IThreadWorker.h"

class IMessageQueueEvents
{
public:
     virtual void on_start() = 0;
     virtual void on_stop() = 0;
     virtual void on_hwm() = 0;
     virtual void on_lwm() = 0;
};

class Manager : public IMessageQueueEvents
{
public:
     void on_start() override
     {
          std::cout << "CALL STRAT " << std::endl;
          startSubscribers( true );
     }
     void on_stop() override
     {
          std::cout << "CALL STOP " << std::endl;
          startSubscribers( false );
     }
     void on_hwm() override
     {
          pauseSubscribers( true );
     }
     void on_lwm() override
     {
          pauseSubscribers( false );
     }

     WorkerHandler addToManaged( WorkerHandler worker )
     {
          m_subscribers.emplace_back( WeakWorkerHandler( worker ) );
          return worker;
     }

private:
     void startSubscribers( bool val )
     {
          m_subscribers.remove_if( [ val ]( const WeakWorkerHandler& handler ) { return handler.expired(); } );

          auto caller = [ val ]( WeakWorkerHandler& weak_handler ) { ( weak_handler.lock() )->start( val ); };

          std::for_each( m_subscribers.begin(), m_subscribers.end(), caller );
     }

     void pauseSubscribers( bool val )
     {
          m_subscribers.remove_if( [ val ]( const WeakWorkerHandler& handler ) { return handler.expired(); } );

          auto caller = [ val ]( WeakWorkerHandler& weak_handler ) { ( weak_handler.lock() )->pause( val ); };

          std::for_each( m_subscribers.begin(), m_subscribers.end(), caller );
     }

private:
     std::list< WeakWorkerHandler > m_subscribers;
};

#endif // IMESSAGEQUEUEEVENTS_H
