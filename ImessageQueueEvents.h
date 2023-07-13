#ifndef IMESSAGEQUEUEEVENTS_H
#define IMESSAGEQUEUEEVENTS_H

#include <iostream>
#include <functional>
#include <algorithm>
#include <memory>
#include <list>

class IMessageQueueEvents
{
public:
     virtual void on_start() = 0;
     virtual void on_stop() = 0;
     virtual void on_hwm() = 0;
     virtual void on_lwm() = 0;
};

class MessageQueueEvents : public IMessageQueueEvents
{
public:
     using CallbackFunction = std::function< void( bool ) >;
     using CallbackHandler = std::shared_ptr< CallbackFunction >;
     using WeakCallbackHandler = std::weak_ptr< CallbackFunction >;

     void on_start() override
     {
          std::cout << "CALL STRAT " << std::endl;
          triggerSubscribers( m_subscribersWork, true );
     }
     void on_stop() override
     {
          std::cout << "CALL STOP " << std::endl;
          triggerSubscribers( m_subscribersWork, false );
     }
     void on_hwm() override
     {
          triggerSubscribers( m_subscribersPause, false );
     }
     void on_lwm() override
     {
          triggerSubscribers( m_subscribersPause, false );
     }

     void addToManaged( std::weak_ptr< CallbackFunction > )
     {}

     MessageQueueEvents::CallbackHandler subscribeToPause( const CallbackFunction& callback )
     {
          auto handler = std::make_shared< CallbackFunction >( callback );
          m_subscribersPause.emplace_back( std::weak_ptr< CallbackFunction >( handler ) );
          return handler;
     }
     MessageQueueEvents::CallbackHandler subscribeToWork( const CallbackFunction& callback )
     {
          auto handler = std::make_shared< CallbackFunction >( callback );
          m_subscribersWork.emplace_back( std::weak_ptr< CallbackFunction >( handler ) );
          return handler;
     }

private:
     void triggerSubscribers( std::list< WeakCallbackHandler >& subscribers, bool val )
     {
          subscribers.remove_if( [ val ]( const WeakCallbackHandler& handler ) { return handler.expired(); } );

          auto caller = [ val ]( WeakCallbackHandler& weak_handler ) { ( *weak_handler.lock() )( val ); };

          std::for_each( subscribers.begin(), subscribers.end(), caller );
     }

private:
     std::list< WeakCallbackHandler > m_subscribersWork;
     std::list< WeakCallbackHandler > m_subscribersPause;
};

#endif // IMESSAGEQUEUEEVENTS_H
