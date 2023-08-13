#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include "ImessageQueueEvents.h"
#include "IThreadWorker.h"

namespace pkus {

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
          std::cout << "CALL HWM " << std::endl;
          pauseSubscribers( true );
     }
     void on_lwm() override
     {
          std::cout << "CALL LWM " << std::endl;
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

} // namespace pkus

#endif // THREADMANAGER_H
