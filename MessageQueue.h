#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "ImessageQueueEvents.h"
#include "RingBuffer2.h"

#include <atomic>

namespace pkus {

enum class RetCode
{
     OK = 0,
     HWM = -1,
     NO_SPACE = -2,
     STOPPED = -3,
     NO_DATA = -4,
};

template< typename T >
class MessageQueue
{
public:
     explicit MessageQueue( int queue_size, int lwm, int hwm )
          : m_container( RingBuffer< T >( queue_size ) )
          , m_hwm( hwm )
          , m_lwm( lwm )
          , m_isStrted( false ) {};

     void setEvents( IMessageQueueEventsPtr events )
     {
          m_events = events;
     };

     void run()
     {
          if( !m_events )
               return;
          m_isStrted = true;
          m_events->on_start();
     }
     void stop()
     {
          if( !m_events )
               return;
          m_isStrted = false;
          m_events->on_stop();
     };
     RetCode put( const T& message )
     {
          if( !m_isStrted )
               return RetCode::STOPPED;

          std::lock_guard< std::mutex > lk( m_mutex );
          try
          {
               m_container.pushBack( message );
          }
          catch( const std::range_error& ex )
          {
               return RetCode::NO_SPACE;
          }

          std::cout << "PUT size " << m_container.size() << std::endl;
          if( m_container.size() >= m_hwm )
          {
               m_events->on_hwm();
               return RetCode::HWM;
          }
          return RetCode::OK;
     }
     RetCode get( T& message )
     {
          if( !m_isStrted )
               return RetCode::STOPPED;

          if( !m_container.size() )
               return RetCode::NO_DATA;

          std::lock_guard< std::mutex > lk( m_mutex );
          message = m_container.popFront();
          std::cout << "GET size " << m_container.size() << std::endl;
          if( m_container.size() <= m_hwm )
          {
               m_events->on_lwm();
          }
          return RetCode::OK;
     }

private:
     IMessageQueueEventsPtr m_events;
     RingBuffer< T > m_container;

     std::atomic_bool m_isStrted;
     std::mutex m_mutex;
     std::uint32_t m_hwm, m_lwm; // HighWaterMark, LowWaterMark
};

template< typename T >
using MessageQueuePtr = ::std::shared_ptr< MessageQueue< T > >;

} // namespace pkus

#endif // MESSAGEQUEUE_H
